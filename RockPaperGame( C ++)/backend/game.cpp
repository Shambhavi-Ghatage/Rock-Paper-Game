
#include "game.h"
#include <cstdlib>
#include <ctime>

using namespace std;

// Constructor: initialize scores
RockPaperScissors::RockPaperScissors()
{
    userScore = 0;
    computerScore = 0;
    drawScore = 0;

    srand(static_cast<unsigned int>(time(nullptr)));
}

// Check whether the choice is valid
bool RockPaperScissors::isValidChoice(string choice)
{
    return choice == "rock" ||
           choice == "paper" ||
           choice == "scissors";
}

// Play one round
GameResult RockPaperScissors::play(string choice)
{
    GameResult game;

    game.userChoice = choice;

    string choices[] = {"rock", "paper", "scissors"};
    game.computerChoice = choices[rand() % 3];

    // Decide the winner
    if (choice == game.computerChoice)
    {
        game.result = "It's a Draw!";
        drawScore++;
    }
    else if (
        (choice == "rock" && game.computerChoice == "scissors") ||
        (choice == "paper" && game.computerChoice == "rock") ||
        (choice == "scissors" && game.computerChoice == "paper")
    )
    {
        game.result = "You Win!";
        userScore++;
    }
    else
    {
        game.result = "Computer Wins!";
        computerScore++;
    }

    // Return updated scores
    game.userScore = userScore;
    game.computerScore = computerScore;
    game.drawScore = drawScore;

    return game;
}

// Reset all scores
void RockPaperScissors::reset()
{
    userScore = 0;
    computerScore = 0;
    drawScore = 0;
}
