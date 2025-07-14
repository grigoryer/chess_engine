#pragma once
#include "types.hpp"

struct ZobristRandoms
{
    U64 rnd_pieces[NUM_SIDES][NUM_PIECES][NUM_SQUARES];
    U64 rnd_castling[NUM_CASTLING];
    U64 rnd_sides[NUM_SIDES];
    U64 rnd_ep[NUM_EPSQUARES];

    ZobristRandoms();

    U64 piece(int side, int piece, int square) const {
        return rnd_pieces[side][piece][square];
    }
    
    U64 castling(U8 castling_permissions) const {
        return rnd_castling[castling_permissions];
    }
    
    U64 side(int side) const {
        return rnd_sides[side];
    }
    
    U64 en_passant(U8 en_passant_square) const {
        return rnd_ep[en_passant_square]; 
    }

};
