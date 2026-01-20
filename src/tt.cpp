#include "tt.hpp"
#include "constants.hpp"
#include <cstring>

TTEntry::TTEntry() 
: hash(0), depth(0), score(NEG_INF), type(LOW), bestMove()
{  }


TTEntry::TTEntry(Key hash, U8 depth, Score score, NodeType type, Move move) 
: hash(hash), depth(depth), score(score), type(type), bestMove(move)
{  }

//Using buckets, add to the first empty bucket entry, if same hash replace with higher depth, and if all fild and a new hash comes we replace lowest depth.
void TTable::addEntry(Key hash, U8 depth, Score score, NodeType type, Move move)
{
    table.at(hash % TTABLE_SIZE) = {hash,depth,score,type,move};
}

TTEntry* TTable::probeEntry(Key hash)
{
    int index = hash % TTABLE_SIZE;
    if(table.at(index).hash == hash)
    {
        return &table.at(index);
    }

    return nullptr;
}


void TTable::resetTable()
{
    for(auto& entry : table)
    {
        entry.hash = 0;
    }
}