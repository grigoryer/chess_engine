#include "constants.hpp"
#include "debug.hpp"
#include <make_move.hpp>


void castlingMoveHelper(Board& b, Side s, Square kingFrom, Square kingTo)
{
    const bool isKingside = (kingTo > kingFrom);

    U8 rookFrom = 0;
    U8 rookTo = 0;

    if (s == WHITE) 
    {
        rookFrom = isKingside ? h1 : a1;
        rookTo = isKingside ? f1 : d1;
        Castling perm = static_cast<Castling>(b.curState.castlingRights & ~Castling::WHITE);
        updateCastlingPerm(b, perm);
    } 
    else 
    {
        rookFrom = isKingside ? h8 : a8;
        rookTo = isKingside ? f8 : d8;
        Castling perm = static_cast<Castling>(b.curState.castlingRights & ~Castling::BLACK);
        updateCastlingPerm(b, perm);
    }

    movePiece(b, s, ROOK, rookFrom, rookTo);
}

void doMove(Board& b, ExtdMove* move)
{
    Side s = b.curSide;
    Square from = move->getFrom();
    Square to = move->getTo();
    Piece piece = move->getPiece();
    b.stateHistory[b.historyCount++] = b.curState;

    if(move->isPromoted())
    {
        removePiece(b, s, piece, from);
        piece = move->getPromoted();
    }

    if(move->isCapture()) 
    { 
        UniquePiece fullPiece = b.pieceList[to];
        Piece pieceType = getPieceType(fullPiece);
        removePiece(b, s ^ 1, pieceType, to);
    }

    if(move->isDouble())
    {
        setEpsquare(b, squareToEpsquare(to, s));
        movePiece(b, b.curSide, piece, from, to);
        swapSides(b);
        return;
    }

    if(move->isCastle())
    {
        castlingMoveHelper(b, s, from, to);
        movePiece(b, b.curSide, piece, from, to);
        swapSides(b);
        return;
    }

    movePiece(b, b.curSide, piece, from, to);
    swapSides(b);
}