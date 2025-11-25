#pragma once
#include <position.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <iostream>
#include <constants.hpp>
#include <evaluation.hpp>





const int NEG_INF = -1e9;
const int POS_INF =  1e9;
int const MAX = 99999;
int const MIN = -99999;
const int MATE_SCORE = 100000;

class Search
{

    Evaluation eval;

    int negaMax(Board& b, int depthLeft, int alpha, int beta, int initialDepth);

public:
    ExtdMove* search(Board& b, int depth);

};


