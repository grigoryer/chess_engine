#include "tt.hpp"
#include "constants.hpp"
#include <cstddef>
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
    const size_t index = hash & (TTABLE_SIZE - 1);
    U8 lowestDepth = 50;
    size_t replaceIndex = 0;

    for (int i = 0; i < TTABLE_BUCKET_SIZE; ++i)
    {

        if (table.at(index).at(i).hash == 0)
        {
            table.at(index).at(i) = {hash,depth,score,type,move};   
            return;
        }
        
        if (table.at(index).at(i).hash == hash)
        {
            if (table.at(index).at(i).depth <= depth)
            {
                table.at(index).at(i) = {hash,depth,score,type,move};   
            }
            return;
        }

        if (table.at(index).at(i).depth < lowestDepth)
        {
            lowestDepth = table.at(index).at(i).depth;
            replaceIndex = i;
        }
    }

    table.at(index).at(replaceIndex) = {hash,depth,score,type,move};
}

TTEntry* TTable::probeEntry(Key hash)
{
    const int index = hash & (TTABLE_SIZE - 1);

    for (int i = 0; i < TTABLE_BUCKET_SIZE; ++i)
    {
        if(table.at(index).at(i).hash == hash)
        {
            return &table.at(index).at(i);
        }
    }

    return nullptr;
}

void TTable::resetTable()
{
    for(auto& entry : table)
    {
        for (int i = 0; i < TTABLE_BUCKET_SIZE; ++i)
        {
            entry.at(i).hash = 0;
        }
    }
}