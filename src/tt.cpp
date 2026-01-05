#include "tt.hpp"
#include "constants.hpp"
#include <cstring>



TTEntry::TTEntry() 
: hash(0), depth(0), score(NEG_INF), type(EXACT), bestMove()
{  }


TTEntry::TTEntry(Key hash, U8 depth, int score, NodeType type, Move move) 
: hash(hash), depth(depth), score(score), type(type), bestMove(move)
{  }


void TTable::addEntry(Key hash, U8 depth, int score, NodeType type, Move move)
{
    TTEntry newEntry{hash, depth, score, type, move};
    table.at(hash % TTABLE_SIZE) = newEntry;     
}

TTEntry* TTable::probeEntry(Key hash)
{
    return &table.at((hash % TTABLE_SIZE));
}


void TTable::resetTable()
{
    for(auto& i : table)
    {
        i.hash = 0;
    }
}