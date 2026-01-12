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
    int index = hash % TTABLE_SIZE;
    TTEntry newEntry{hash, depth, score, type, move};

    //default depth and index.
    int lowestDepth = 256;
    int replaceIndex = 0;

    for(int bucketIndex = 0; bucketIndex < TTABLE_BUCKET_SIZE; bucketIndex++)
    {
        //if empty bucket entry add and return;
        if(table.at(index).bucket.at(bucketIndex).hash == 0)
        {
            table.at(index).bucket.at(bucketIndex) = newEntry;
            return;
        }

        //same index replace always
        if(table.at(index).bucket.at(bucketIndex).hash == hash)
        {    
            table.at(index).bucket.at(bucketIndex) = newEntry;
            return;
        }

        /*if(table.at(index).bucket.at(bucketIndex).depth < lowestDepth)
        {
            lowestDepth = table.at(index).bucket.at(bucketIndex).depth;
            replaceIndex = bucketIndex;
        }*/
    }

    table.at(index).bucket.at(0) = newEntry;
}

TTEntry* TTable::probeEntry(Key hash)
{
    int index = hash % TTABLE_SIZE;

    for(int i = 0; i < TTABLE_BUCKET_SIZE; i++)
    {
        if( table.at(index).bucket.at(i).hash == hash)
        {
            return &table.at(index).bucket.at(i);
        }
    }
    return nullptr;
}


void TTable::resetTable()
{
    for(auto& bucket : table)
    {
        for(int i = 0; i < TTABLE_BUCKET_SIZE; i++)
        {
            bucket.bucket.at(i).hash = 0;
            bucket.bucket.at(i).depth = 0;
            bucket.bucket.at(i).score = NEG_INF;
            bucket.bucket.at(i).type = LOW;
        }
    }
}