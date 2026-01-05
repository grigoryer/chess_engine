#pragma once
#include <position.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <constants.hpp>
#include <evaluation.hpp>


/*
Use the transposition table too check previous node type and updated alpha beta from there.

EXACT or PV_NODE means node is within the bound and is greater than alpha but less than beta, needs to be evaluated.
return score immeidatly since we know the true score of the entire tree from here.

fail HIGH is a cut node or fail-high node this node exceed beta thus does not need to be evaluted the score returned from this 
is a lower bound, meaning score is atelast this but can be higher since potential better positions

fail LOW is a all node or fail low node this node doesnt exceed alpha so nothing happens on this node
it is a upper bound, since it assumes we checked all the nodes 



Psuedo code for implementation

if depth left = 0
    return evaluation
if draw 
    return 0

if isPositon in TT
    ttentry = stored position
    if ttentry.depth < depth
        continue;
    else if ttentry.type == exact
        return score
    else if ttentry.type == HIGH && ttentry.score >= beta
        return score
    else if ttentry.type == LOW && ttentry.score =< alpha
        return score


generate moves and order

if legal count = 0
    return //(either mate or stalemate)


best score = negative infinity

originalAlpha = alpha
NodeType type;

for each move
    do move
    score = search(deppt - 1, -beta, -alpha)
    undo move

    if score >= beta
        nodeType = HIGH
        addEntry to Transpositon Table
        return score 
    if score > best score
        bestScore = score
    if score > alpha 
        alpha = score

//outside of loop


if(bestScore <= originalAlpha)
    nodeType = LOW
else
    nodeType = EXACT

addEntry to Transpositon Table
(keep one with longer depth)

return bestScore
*/

enum NodeType : U8
{
    EXACT, HIGH, LOW 
};

struct TTEntry
{
    Key hash = 0ULL;
    U8 depth = 0;
    int score = 0;
    NodeType type = EXACT;
    Move bestMove{};

    TTEntry();
    TTEntry(Key hash, U8 depth, int score, NodeType type, Move best_move);
};

class TTable
{
    std::array<TTEntry, TTABLE_SIZE> table;
    size_t count;
    
public:
    void resetTable();
    void addEntry(Key hash, U8 depth, int score, NodeType type, Move best_move);
    void manageCollision(Key hash, U8 depth, Score score, NodeType type, Move move);
    TTEntry* probeEntry(Key hash);

};



