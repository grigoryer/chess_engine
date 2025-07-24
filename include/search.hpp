#pragma once
#include "bitboards.hpp"
#include "eval.hpp"
#include "types.hpp"



class Search {
public:
    Board& board;
    Move best_move_found;
    
public:
    Search(Board& b) : board(b) {}

    int search(int depth);
};