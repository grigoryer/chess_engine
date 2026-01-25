#include "constants.hpp"
#include "move_generation.hpp"
#include "moves.hpp"
#include "tt.hpp"
#include <algorithm>
#include <iostream>
#include <array>
#include <search.hpp>


ExtdMove Search::iterativeDeep(Board& b, const int maxDepth)
{
    ExtdMove bestMove{};
    ExtdMove tempMove{};

    int depth = 1;
    while (!stopFlag.load() && depth <= maxDepth) 
    {
        Board copy = b; // for now copy since getting king disapperaing error TODO: find out why king disapperas in original board state.
        
        tempMove = search(copy, depth);

        if (stopFlag.load()) { break; }
        
        bestMove = tempMove; // only update if search completed we select only best move if the search is completed 

        // info with depth, nodes searched and score of best position possible
        std::cout << "info depth " << depth  
        << " move " << SQUARE_NAMES[bestMove.getFrom()] << SQUARE_NAMES[bestMove.getTo()] 
        << " cp score " << selectedDepthScore 
        << " nodes " << nodesSearched 
        << " TT hit rate: " << (100.0 * ttHits / ttProbes) << "%"
        << std::endl;   

        depth++;
    }
    
    return bestMove;
}


// helper function returns best move at depth 1 perfroming negamax on all rest depth nodes 
ExtdMove Search::search(Board& b, const int depth)
{
    // reset searching stats and default move
    nodesSearched = 0;
    selectedDepthScore = 0;
    ttHits = 0;
    ttProbes = 0;

    ExtdMove NULL_MOVE;
    NULL_MOVE.setMove(noSquare, noSquare, KING);

    if (b.isDraw()) { return NULL_MOVE; } // draw check for reptition or 50 moves

    // gen legal moves, then get amount of legal moves and sort them using move ordering MVV LVA
    MoveList list{};
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end); // returns legal count/scores moves (illegals get pushed to back)/and orders them,

    if (legalCount == 0) { return NULL_MOVE; } // mate or stalemate since no legal moves

    ExtdMove bestMove = NULL_MOVE;
    int bestScore = NEG_INF;
    
    // for each move check if we need check stoppage, then recursivly search through move tree negamax.
    for (auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {
        if (stopFlag.load()) { return bestMove; }

        doMove(b, m);
        int score = -negaMax(b, depth - 1, NEG_INF, POS_INF, depth);
        undoMove(b, m);

        // update best move if score exceeds prev
        if (score > bestScore) 
        {
            bestScore = score;
            bestMove = *m;
            selectedDepthScore = score;
        }   
    }
    
    return bestMove;
}

int Search::negaMax(Board& b, int depthLeft, int alpha, int beta, const int& intitialDepth)
{
    // update node and check if stop flag active, since we havent completed it we return neg_inf to discard the search
    nodesSearched++;
    if (((nodesSearched & 2047) == 0) && stopFlag.load()) // check every 2048 nodes
    {
        return NEG_INF;
    }

    if (b.isDraw()) { return 0; }

    // TT probe
    ttProbes++;
    TTEntry* ttEntry = tranposTable->probeEntry(b.curState.hash);

    if (ttEntry != nullptr && ttEntry->hash == b.curState.hash)
    {
        if (ttEntry->depth >= depthLeft) 
        {
            if (ttEntry->type == EXACT)
            {
                ttHits++;
                return ttEntry->score;
            }
            else if (ttEntry->type == HIGH && ttEntry->score >= beta)
            {
                ttHits++;
                return ttEntry->score;
            }
            else if (ttEntry->type == LOW && ttEntry->score <= alpha)
            {
                ttHits++;
                return ttEntry->score;
            }
        }
    }
    
    if (depthLeft == 0) { return searchQuiescence(b, QSEARCH_DEPTH, alpha, beta); } //change to Q-search

    MoveList list{};
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end);

    if (legalCount == 0)
    {
        if (b.isCheck(b.curSide)) { return -MATE; } // mate return mate score, the way it is scored and propogated up will change on ply
        else { return 0; }  // stalemate
    }

    int bestScore = NEG_INF;
    ExtdMove bestMove{};
    Key startHash = b.curState.hash;
    
    int alphaOriginal = alpha; // store for tt entry
    NodeType nodeType{};

    for (auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {
        if (stopFlag.load()) { return bestScore; }

        doMove(b, m);
        int score = -negaMax(b, depthLeft - 1, -beta, -alpha, intitialDepth);
        undoMove(b, m);

        // make score relevant to the position for transposition
        if (score > MATE - 1000) { score--; } 
        else if (score < -MATE + 1000) { score++; }

        // alphabeta cutoffs, add ttentry here for beta since it is a cutoff
        if (score >= beta) 
        { 
            nodeType = HIGH;
            if (!stopFlag.load()) { tranposTable->addEntry(b.curState.hash, depthLeft, score, HIGH, *m); }
            return score; 
        }
        
        if (score > bestScore) 
        { 
            bestScore = score; 
            bestMove = *m;
        }
        
        if (score > alpha) { alpha = score; } 
    }
    
    // dont add recent entry to TT if we called a stop since we return NEG_INF for null moves.
    if (!stopFlag.load())
    {
        if (bestScore <= alphaOriginal) 
        { 
            nodeType = LOW; 
        }
        else 
        { 
            nodeType = EXACT; 
        }
        
        tranposTable->addEntry(b.curState.hash, depthLeft, bestScore, nodeType, bestMove);
    }

    return bestScore;
}

// DOESNT WORK: TODO, generateQuiescence doesnt work
int Search::searchQuiescence(Board& b, int depthLeft, int alpha, int beta)
{
    nodesSearched++;
    
    //probe tt for 
    int bestValue = (b.curSide == WHITE ? eval.evaluateBoard(b) : -eval.evaluateBoard(b));

    if (depthLeft == 0 || ((nodesSearched & 2047) == 0 && stopFlag.load())) { return bestValue; }

    if (bestValue >= beta) { return bestValue; }
    if (bestValue > alpha) { alpha = bestValue; }

    MoveList list{};
    auto end = generateQuiescence(list.list.begin(), b, b.curSide);
    int legalCount = scoreMoveList(b, list, end);

    for (auto m = list.list.begin(); m < list.list.begin() + legalCount; ++m)
    {
        if (stopFlag.load()) { return bestValue; }

        doMove(b, m);
        int score = -searchQuiescence(b, depthLeft - 1, -beta, -alpha);
        undoMove(b, m);

        if (stopFlag.load()) { return bestValue; }

        if (score >= beta) { return score; }
        if (score > bestValue) { bestValue = score; }
        if (score > alpha) { alpha = score; } 
    }

    return bestValue;
}


// scoreMoveList checks stopFlag as well
int Search::scoreMoveList(Board& b, MoveList& list, ExtdMove* end)
{
    int legalCount{0};

    ExtdMove* begin = list.list.data();
    Bitboard blockers = generateBlockers(b, b.curSide);

    Move ttMove{};
    TTEntry* entry = tranposTable->probeEntry(b.curState.hash);
    if (entry != nullptr)
    {
        ttMove = entry->bestMove;
    }

    for (auto m = list.list.begin(); m != end; ++m)
    {
        // if its a legal move we score otherwise it stays at 0 score;
        if (isLegal(m, b, b.curSide, blockers))
        {
            legalCount++;
            m->scoreMove();
        }

        // if move is the one stored in TTtable, we can add the score 
        if (entry != nullptr && m->getFrom() == ttMove.getFrom() && m->getTo() == ttMove.getTo()) 
        {
            m->score += 5000;
        }
    }

    std::sort(begin, end, [](const ExtdMove& a, const ExtdMove& b)
    {
        return a.score > b.score;
    });

    return legalCount;
}