#pragma once
#include "types.hpp"
#include "bitboards.hpp"
#include <array>


const std::array<U16, NUM_PIECES> piece_values = {0, 900, 500, 350, 325, 100};

int evaluate(Board& board);


int count_material(U8 side, Board& board);