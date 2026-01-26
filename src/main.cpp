#include "attacks.hpp"
#include "uci.hpp"
#include <hash.hpp>
#include <debug.hpp>
#include <iostream>
#include <moves.hpp>
#include <constants.hpp>
#include <position.hpp>
#include <between.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <search.hpp>
#include <evaluation.hpp>

void initNameSpaces()
{
    Attacks::initializeAttacks();
    Between::initializeBetween();
    ZobristHashing::initializeZobristRandoms();
}


int main()
{
    UciEngine game{};
    initNameSpaces();
    game.run();
}

