#pragma once
#include <memory>
#include <position.hpp>
#include <tt.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <atomic>
#include <constants.hpp>
#include <evaluation.hpp>


int scoreMoveList(Board& b, MoveList& list, ExtdMove* end);


class Search
{
    int nodesSearched = 0;
    int selectedDepthScore = 0;
    Evaluation eval;
    int negaMax(Board& b, int depthLeft, int alpha, int beta, const int& initialDepth);
    int nextMove(Move& list, int count);
    int searchQuiescence(Board& b, int depthLeft, int alpha, int beta);
    int scoreMoveList(Board& b, MoveList& list, ExtdMove* end);

public:

    std::unique_ptr<TTable> tranposTable = std::make_unique<TTable>();
    std::atomic<bool> stopFlag{false};
    ExtdMove search(Board& b, const int depth);
    ExtdMove iterativeDeep(Board& b, const int maxDepth);
};


