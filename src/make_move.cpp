#include "constants.hpp"
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

void finishMoveAction(Board& b, Piece piece, Square from, Square to, bool hmFlag)
{
    if(hmFlag) 
    {
        b.curState.halfmoveCount++;
    }
    else 
    {
        b.curState.halfmoveCount = 0; 
    }

    if(b.curSide == BLACK) { b.curState.fullmoveCount++; }

    movePiece(b, b.curSide, piece, from, to);

    if(piece == KING) { b.setCheckSqs(b.curSide); }
    swapSides(b);
}

void doMove(Board& b, ExtdMove* move)
{
    bool hmFlag = true;
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
        finishMoveAction(b, piece, from, to, hmFlag);
        return;
    }

    if(move->isDouble())
    {
        setEpsquare(b, squareToEpsquare(to, s));
        finishMoveAction(b, piece, from, to, hmFlag);
        return;
    }

    if(move->isCastle())
    {
        castlingMoveHelper(b, s, from, to);
        finishMoveAction(b, piece, from, to, hmFlag);
        return;
    }
 
    finishMoveAction(b, piece, from, to, hmFlag);
}


void castlingUndoHelper(Board& b, Side s, Square kingFrom, Square kingTo)
{
    const bool isKingside = (kingTo > kingFrom);
    U8 rookFrom = 0;
    U8 rookTo = 0;
    
    if (s == WHITE) 
    {
        rookFrom = isKingside ? h1 : a1;
        rookTo = isKingside ? f1 : d1;
    } 
    else 
    {
        rookFrom = isKingside ? h8 : a8;
        rookTo = isKingside ? f8 : d8;
    }
    
    movePiece(b, s, ROOK, rookTo, rookFrom);
}

void undoMoveAction(Board& b, Piece piece, Square from, Square to)
{
    movePiece(b, b.curSide, piece, to, from);
    if(piece == KING) { b.setCheckSqs(b.curSide); }
}

void undoMove(Board& b, ExtdMove* move)
{

    swapSides(b);
    b.historyCount--;
    Side s = b.curSide;
    Square from = move->getFrom();
    Square to = move->getTo();
    Piece piece = move->getPiece();

    if(move->isCapture()) 
    { 
        UniquePiece fullPiece = b.pieceList[to];
        Piece capturedPiece = getPieceType(fullPiece);
        undoMoveAction(b, piece, from, to);
        putPiece(b, s ^ 1, capturedPiece, to);
        b.curState = b.stateHistory[b.historyCount];
        return;
    }

    if(move->isCastle())
    {
        castlingUndoHelper(b, s, from, to);
        undoMoveAction(b, piece, from, to);
        b.curState = b.stateHistory[b.historyCount];
        return;
    }

    if(move->isPromoted())
    {
        piece = PAWN;
    }

    undoMoveAction(b, piece, from, to);
    b.curState = b.stateHistory[b.historyCount];
}