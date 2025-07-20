#include "zobrist.hpp"
#include <random>
#include <ctime>

ZobristRandoms::ZobristRandoms() //NOLINT
{
    std::mt19937_64 random(time(0));


    for(U8 i = 0; i < NUM_SIDES; ++i)
    {
        rnd_sides[i] = random();

        for(U8 j = 0; j < NUM_PIECES; ++j)
        {
            for(U8 k = 0; k < NUM_SQUARES; ++k)
            {
                rnd_pieces[i][j][k] = random();
            }
        }
    }

    for(U8 i = 0; i < NUM_CASTLING; ++i)
    {
        rnd_castling[i] = random();
    }

    for(U8 i = 0; i < NUM_EPSQUARES; ++i)
    {
        rnd_ep[i] = random();
    }
}

U64 ZobristRandoms::en_passant(U8 en_passant_square) const {
    return rnd_ep[en_passant_square]; 
}

U64 ZobristRandoms::piece(U8 side, U8 piece, U8 square) const {
    return rnd_pieces[side][piece][square];
}

U64 ZobristRandoms::castling(U8 castling_permissions) const {
    return rnd_castling[castling_permissions];
}
    
U64 ZobristRandoms::side(U8 side) const {
    return rnd_sides[side];
}