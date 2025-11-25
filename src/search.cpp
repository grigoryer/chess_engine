#include <search.hpp>


ExtdMove* Search::search(Board& b, int depth)
{
    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    Bitboard blockers = generateBlockers(b, b.curSide);

    int legalCount = 0;
    ExtdMove* bestMove = nullptr;
    int bestScore = NEG_INF;

    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, b, b.curSide, blockers)) { continue; }

        legalCount++;

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
    if(legalCount == 0)
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


    return bestMove;

}


int Search::negaMax(Board& b, int depthLeft, int alpha, int beta, int initialDepth)
{
    if(depthLeft == 0) { return (b.curSide == WHITE ? eval.evaluateBoard(b) : -eval.evaluateBoard(b)); }

    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    Bitboard blockers = generateBlockers(b, b.curSide);

    int legalCount = 0;
    int bestScore = NEG_INF;

    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, b, b.curSide, blockers)){ continue; }
        legalCount++;

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
    if(legalCount == 0)
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
