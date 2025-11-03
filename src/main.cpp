#include "attacks.hpp"
#include <iostream>
#include <constants.hpp>



int main()
{
    Attacks::initialize();

    for(int i = a1; i < noSquare; i++)
    {
        std::cin.get();
        printBoard(Attacks::pawnAttacks[BLACK][i]);
    }
    return 0;
}