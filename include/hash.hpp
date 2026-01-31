#pragma once
#include "constants.hpp"
#include "position.hpp"
#include <ctime>

namespace ZobristHashing
{
    void initializeZobristRandoms();
    Key zobPiece(Side s, Piece piece, Square sq);
    Key zobCastling(Castling castlingPerm);
    Key zobSide(Side s);
    Key zobEnpassant(EpSquare epSq);
    Key initailizeHash(Board &b);
};
