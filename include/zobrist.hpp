#pragma once
#include "bitboards.hpp"

struct ZobristRandoms
{
    U64 rnd_pieces[2][6][64];
    U64 rnd_castling[16];
    U64 rnd_sides[2];
    U64 rnd_ep[17];

    ZobristRandoms();
};