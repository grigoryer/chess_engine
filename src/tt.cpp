#include "tt.hpp"
#include "constants.hpp"
#include <cstring>

TTEntry::TTEntry() 
: hash(0), depth(0), score(NEG_INF), type(LOW), bestMove()
{  }

TTEntry::TTEntry(Key hash, U8 depth, Score score, NodeType type, Move move) 
: hash(hash), depth(depth), score(score), type(type), bestMove(move)
{  }

//replace if new depth is greater
void TTable::addEntry(Key hash, U8 depth, Score score, NodeType type, Move move)
{
    const int index = hash & (TTABLE_SIZE - 1);

    if(table.at(index).hash == hash)
    {
        if(table.at(index).depth <= depth)
        {
            table.at(index) = {hash,depth,score,type,move};   
        }
    }
    else 
    {
        table.at(index) = {hash,depth,score,type,move};   
    }
}

TTEntry* TTable::probeEntry(Key hash)
{
    const int index = hash & (TTABLE_SIZE - 1);

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