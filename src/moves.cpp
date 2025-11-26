#include "constants.hpp"
#include <moves.hpp>


const std::array<std::array<int, NUM_PIECES>, NUM_PIECES> MVV_LVA_TABLE
{
    0, 0, 0, 0, 0, 0, 
    0, 8100, 8500, 8685, 8700, 8900, 
    0, 4100, 4500, 4685, 4700, 4900, 
    0, 2250, 2650, 2835, 2850, 3050, 
    0, 2100, 2500, 2685, 2700, 2900, 
    0, 100, 500, 685, 700, 900, 
};



void ExtdMove::scoreMove()
{
    int tempScore = 1000;

    if(getCapture() != NONE) 
    { 
        tempScore += MVV_LVA_TABLE[getCapture()][getPiece()];
    }

    score = tempScore;
}