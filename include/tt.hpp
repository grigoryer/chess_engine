#pragma once
#include <position.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <constants.hpp>
#include <evaluation.hpp>


enum NodeType : U8
{
    EXACT, UPPER, LOWER 
};

struct TTEntry
{
    Key hash = 0ULL;
    U8 depth = 0;
    Score score = 0;
    NodeType type = EXACT;
    Move bestMove{};

    TTEntry();
    TTEntry(Key hash, U8 depth, Score score, NodeType type, Move best_move);
};

class TTable
{
    std::array<TTEntry, TTABLE_SIZE> table;
    size_t count;

    void addEntry(int index, Key hash, U8 depth, Score score, NodeType type, Move best_move);
    bool checkHit(Key hash);
    TTEntry* probe(uint64_t key);

    TTable();
};



