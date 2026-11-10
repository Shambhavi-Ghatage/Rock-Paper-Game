#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "game.h"

using namespace std;

RockPaperScissors game;

string createJSON(const GameResult &result)
{
    stringstream ss;

    ss << "{"
       << "\"user_choice\":\"" << result.userChoice << "\","
       << "\"computer_choice\":\"" << result.computerChoice << "\","
       << "\"result\":\"" << result.result << "\","
       << "\"user_score\":" << result.userScore << ","
       << "\"computer_score\":" << result.computerScore << ","
       << "\"draw_score\":" << result.drawScore
       << "}";

    return ss.str();
}

string getRequestBody(const string &request)
{
    size_t pos = request.find("\r\n\r\n");

    if (pos == string::npos)
        return "";

    return request.substr(pos + 4);
}

string getChoice(const string &body)
{
    size_t pos = body.find("\"choice\"");

    if (pos == string::npos)
        return "";

    pos = body.find(':', pos);

    if (pos == string::npos)
        return "";

    size_t start = body.find('"', pos + 1);

    if (start == string::npos)
        return "";

    size_t end = body.find('"', start + 1);

    if (end == string::npos)
        return "";

    string choice = body.substr(start + 1, end - start - 1);

    for (char &ch : choice)
        if (ch >= 'A' && ch <= 'Z')
            ch = ch - 'A' + 'a';

    return choice;
}

void sendResponse(SOCKET client, int status, const string &body)
{
    string statusText = status == 200 ? "200 OK" :
                        status == 400 ? "400 Bad Request" :
                        status == 404 ? "404 Not Found" : "500 Error";

    string response =
        "HTTP/1.1 " + statusText + "\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Content-Length: " + to_string(body.size()) + "\r\n"
        "Connection: close\r\n\r\n" + body;

    send(client, response.c_str(), (int)response.size(), 0);
}

int main()
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSocket, (sockaddr *)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Could not use port 8080. Close any other server using it."
             << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Server could not start listening." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Rock Paper Scissors server started!" << endl;
    cout << "Server URL: http://localhost:8080" << endl;
    cout << "Keep this terminal open while playing." << endl;

    while (true)
    {
        SOCKET client = accept(serverSocket, NULL, NULL);

        if (client == INVALID_SOCKET)
            continue;

        char buffer[8192] = {0};
        int received = recv(client, buffer, sizeof(buffer) - 1, 0);

        if (received <= 0)
        {
            closesocket(client);
            continue;
        }

        string request(buffer, received);

        if (request.find("OPTIONS ") == 0)
        {
            sendResponse(client, 200, "");
        }
        else if (request.find("POST /play ") == 0)
        {
            string body = getRequestBody(request);
            string choice = getChoice(body);

            if (choice != "rock" &&
                choice != "paper" &&
                choice != "scissors")
            {
                sendResponse(client, 400,
                    "{\"error\":\"Choose rock, paper, or scissors.\"}");
            }
            else
            {
                GameResult result = game.play(choice);
                sendResponse(client, 200, createJSON(result));
            }
        }
        else if (request.find("POST /reset ") == 0)
        {
            game.reset();
            sendResponse(client, 200,
                "{\"user_score\":0,\"computer_score\":0,\"draw_score\":0}");
        }
        else
        {
            sendResponse(client, 404,
                "{\"error\":\"Endpoint not found.\"}");
        }

        closesocket(client);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}