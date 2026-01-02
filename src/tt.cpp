#include "tt.hpp"


TTEntry::TTEntry(Key hash, U8 depth, Score score, NodeType type, Move move) :
hash(hash), depth(depth), score(score), 
type(type), bestMove(move)
{  }



void TTable::addEntry(int index, Key hash, U8 depth, Score score, NodeType type, Move move)
{
    TTEntry newEntry{hash,depth, score, type, move};
    table.at(index) = newEntry;
}

bool TTable::checkHit(Key hash)
{
    
}