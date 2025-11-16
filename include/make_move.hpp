#pragma once 
#include "hash.hpp"
#include <debug.hpp>
#include <constants.hpp>
#include <moves.hpp>



void doMove(Board& b, ExtdMove* move);
void undoMove(Board& b, ExtdMove* move);

inline void removePiece(Board& b, Side s, Piece piece, Square sq)
{
    popBit(b.pieceBB[piece], sq);
    popBit(b.sideBB[s], sq);
    popBit(b.occupancy, sq);
    b.pieceList[sq] = UniquePiece::NONE; 
    b.curState.hash ^= ZobristHashing::zobPiece(s, piece, sq);
}

inline void putPiece(Board& b, Side s, Piece piece, Square sq)
{
    setBit(b.pieceBB[piece], sq);
    setBit(b.sideBB[s], sq);
    setBit(b.occupancy, sq);
    b.pieceList[sq] = static_cast<UniquePiece>(piece + (s == WHITE ? 0 : NUM_PIECES));

    b.curState.hash ^= ZobristHashing::zobPiece(s, piece, sq);
}

inline void swapSides(Board& b)
{
    b.curState.hash ^= ZobristHashing::zobSide(b.curSide);
    b.curSide ^= 1;
    b.curState.hash ^= ZobristHashing::zobSide(b.curSide);
}

inline void updateCastlingPerm(Board& b, Castling permissions)
{
    b.curState.hash ^= ZobristHashing::zobCastling(b.curState.castlingRights);
    b.curState.castlingRights = permissions;
    b.curState.hash ^= ZobristHashing::zobCastling(b.curState.castlingRights);
}

inline void setEpsquare(Board& b, EpSquare sq)
{
    b.curState.hash ^= ZobristHashing::zobEnpassant(b.curState.epSq);
    b.curState.epSq = sq;
    b.curState.hash ^= ZobristHashing::zobEnpassant(b.curState.epSq);
}

inline void clearEpsquare(Board& b)
{
    b.curState.hash ^= ZobristHashing::zobEnpassant(b.curState.epSq);
    b.curState.epSq = EpSquare::NONE;
    b.curState.hash ^= ZobristHashing::zobEnpassant(EpSquare::NONE);
}

inline void movePiece(Board& b, Side s, Piece piece, Square from, Square to)
{
    removePiece(b, s, piece, from);
    putPiece(b, s, piece, to);
}