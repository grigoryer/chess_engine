#include "constants.hpp"
#include "debug.hpp"
#include "move_generation.hpp"
#include "moves.hpp"
#include <algorithm>
#include <array>
#include <search.hpp>
#include <thread>


ExtdMove Search::iterativeDeep(Board& b, const int maxDepth)
{
    ExtdMove bestMove{};
    ExtdMove tempMove{};

    int depth = 1;
    auto startTime = std::chrono::steady_clock::now();
    
    while(!stopFlag.load() && depth <= maxDepth) 
    {
        Board copy = b;
        tempMove = search(copy, depth);
        
        if(stopFlag.load())
        {
            break;
        }
        
        bestMove = tempMove;  // Only update if search completed
        std::cout << "info depth " << depth  << " move " << SQUARE_NAMES[bestMove.getFrom()]
                << SQUARE_NAMES[bestMove.getTo()] << std::endl;

        depth++;
        //printDebug(b);
    }
    
    return bestMove;
}



// helper function returns best move at depth 1 perfroming negamax on all rest depth nodes 
ExtdMove Search::search(Board& b, const int depth)
{
    ExtdMove NULL_MOVE;
    NULL_MOVE.setMove(noSquare, noSquare, KING);

    if(b.isDraw()) { return NULL_MOVE; } //draw check for reptition or 50 moves

    //gen legal moves, then get amount of legal moves and sort them using move ordering MVVLVA
    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end);

    if(legalCount == 0) { return NULL_MOVE; } //mate or stalemate since no legal moves

    ExtdMove bestMove = NULL_MOVE;
    int bestScore = NEG_INF;
    
    //for each move check if we need check stoppage, then recursivly do move thorugh negamax.
    for(auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {
        if(stopFlag.load()) { return bestMove; }

        doMove(b, m);
        int score = -negaMax(b, depth - 1, NEG_INF, POS_INF, depth);
        undoMove(b, m);

        if(score > bestScore) //update best move if score exceeds prev
        {
            bestScore = score;
            bestMove = *m;
        }
        
        //std::cout << "   info move current " << SQUARE_NAMES[bestMove.getFrom()] << SQUARE_NAMES[bestMove.getTo()] << std::endl;
    }


    //std::cout << "  info BEST SCORE: " << bestScore << std::endl;
    return bestMove;
}

int Search::negaMax(Board& b, int depthLeft, int alpha, int beta, int initialDepth)
{
    if(stopFlag.load()) { return NEG_INF; }

    if(depthLeft == 0) { return searchQuiescence(b, q_depth, alpha, beta); }
    if(b.isDraw()) { return 0; }

    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end);

    if (legalCount == 0)
    {
        if(b.isCheck(b.curSide)) { return NEG_INF + (initialDepth - depthLeft); } //mate return neg_inf + ply
        else { return 0; }  // stalemate
    }

    int bestScore = NEG_INF;

    for (auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {

        //std::cout << "      info move current " << SQUARE_NAMES[m->getFrom()] << SQUARE_NAMES[m->getTo()] << " depth " << depthLeft << std::endl;

        if(stopFlag.load()) { return bestScore; }

        doMove(b, m);
        int score = -negaMax(b, depthLeft - 1, -beta, -alpha, initialDepth);
        undoMove(b, m);

        if(score > bestScore) bestScore = score;
        if(score > alpha) alpha = score;
        if(score >= beta) return score;
    }

    return bestScore;
}

int Search::searchQuiescence(Board& b, int depthLeft, int alpha, int beta)
{
    int bestValue = (b.curSide == WHITE ? eval.evaluateBoard(b) : -eval.evaluateBoard(b));

    if(depthLeft == 0 || stopFlag.load()) return bestValue;

    if(bestValue >= beta) return bestValue;
    if(bestValue > alpha) alpha = bestValue;

    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end);

    for (auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {
        if(stopFlag.load()) return bestValue;

        doMove(b, m);
        int score = -searchQuiescence(b, depthLeft - 1, -beta, -alpha);
        undoMove(b, m);

        if(stopFlag.load()) return bestValue;

        if(score >= beta) return score;
        if(score > bestValue) bestValue = score;
        if(score > alpha) alpha = score;
    }

    return bestValue;
}

// scoreMoveList checks stopFlag as well
int Search::scoreMoveList(Board& b, MoveList& list, ExtdMove* end)
{
    int legalCount = 0;
    ExtdMove* begin = list.list.data();
    Bitboard blockers = generateBlockers(b, b.curSide);

    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(isLegal(m, b, b.curSide, blockers))
        {
            legalCount++;
            m->scoreMove();
        }
    }

    std::sort(begin, end, [](const ExtdMove& a, const ExtdMove& b)
    {
        return a.score > b.score;
    });

    return legalCount;
}
