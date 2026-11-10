
#ifndef GAME_H
#define GAME_H

#include <string>

struct GameResult
{
    std::string userChoice;
    std::string computerChoice;
    std::string result;

    int userScore;
    int computerScore;
    int drawScore;
};

class RockPaperScissors  //C++ class that manages the game.
{
private:
    int userScore;
    int computerScore;
    int drawScore;

public:
    RockPaperScissors();

    GameResult play(std::string choice);   //processes one round.

    void reset();

    bool isValidChoice(std::string choice);   //checks whether the user's choice is valid.
};

#endif
