#pragma once
#include "types.hpp"
#include <array>

struct ZobristRandoms
{
    std::array<std::array<std::array<U64,NUM_SQUARES>,NUM_PIECES>,NUM_SIDES> rnd_pieces;
    std::array<U64, NUM_CASTLING> rnd_castling;
    std::array<U64, NUM_SIDES> rnd_sides;
    std::array<U64, NUM_EPSQUARES> rnd_ep;

    ZobristRandoms();

    U64 piece(U8 side, U8 piece, U8 square) const;

    U64 castling(U8 castling_permissions) const;
    
    U64 side(U8 side) const;
    
    U64 en_passant(U8 en_passant_square) const;

};
