#pragma once
#include <position.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <iostream>
#include <atomic>
#include <constants.hpp>
#include <evaluation.hpp>


int scoreMoveList(Board& b, MoveList& list, ExtdMove* end);


const int NEG_INF = -1e9;
const int POS_INF =  1e9;
int const MAX = 99999;
int const MIN = -99999;
const int MATE_SCORE = 100000;

class Search
{
    Evaluation eval;
    int negaMax(Board& b, int depthLeft, int alpha, int beta, int initialDepth);
    int nextMove(Move& list, int count);
    int searchQuiescence(Board& b, int depthLeft, int alpha, int beta);
    int scoreMoveList(Board& b, MoveList& list, ExtdMove* end);
public:
    std::atomic<bool> stopFlag{false};
    ExtdMove search(Board& b, const int depth);
    ExtdMove iterativeDeep(Board& b, const int maxDepth);
};


