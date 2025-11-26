#include "constants.hpp"
#include "debug.hpp"
#include "move_generation.hpp"
#include "moves.hpp"
#include <algorithm>
#include <array>
#include <search.hpp>


int scoreMoveList(Board& b, MoveList& list, ExtdMove* end);

ExtdMove* Search::search(Board& b, int depth)
{
    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);

    int legalCount = scoreMoveList(b, list, end);

    ExtdMove* bestMove = nullptr;
    int bestScore = NEG_INF;

    if(legalCount == 0 || b.curState.halfmoveCount == 100)
    {
        //mate
        if(b.isCheck(b.curSide))
        {
            return nullptr;
        }
        //stalemate
        else 
        {
            return nullptr;
        }
    }


    for (auto m = list.list.begin(); m <= list.list.begin() + legalCount - 1; ++m)
    {
        doMove(b, m);
        int score = -negaMax(b, depth - 1, NEG_INF, POS_INF, depth);
        undoMove(b, m);

        if(score > bestScore)
        {
           bestScore = score;
           bestMove = m; 
        }

    }


    //if no legal moves in position then check for mate or stalemate
    return bestMove;

}



int Search::negaMax(Board& b, int depthLeft, int alpha, int beta, int initialDepth)
{
    if(depthLeft == 0) { return b.curSide == WHITE ? eval.evaluateBoard(b) : -eval.evaluateBoard(b); }

    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);

    int legalCount = scoreMoveList(b, list, end);
    int bestScore = NEG_INF;


    for (auto m = list.list.begin(); m <  list.list.begin() + legalCount; ++m)
    {
        doMove(b, m);
        int score = -negaMax(b, depthLeft - 1, -beta, -alpha, initialDepth);
        undoMove(b, m);

        if(score > bestScore) 
        { 
            bestScore = score;
            if(score > alpha) { alpha = score; }
        }
        if(score >= beta)
        {
            return score;
        }
    }

    //if no legal moves in position then check for mate or stalemate
    if(legalCount == 0 || b.curState.halfmoveCount == 100)
    {
        //mate
        if(b.isCheck(b.curSide))
        {
            return -POS_INF + (initialDepth - depthLeft);
        }
        //stalemate
        else 
        {
            return 0;
        }
    }

    return bestScore;
}

//returns legal move count;
int scoreMoveList(Board& b, MoveList& list, ExtdMove* end)
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

