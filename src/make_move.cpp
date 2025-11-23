#include "attacks.hpp"
#include "between.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "moves.hpp"
#include <make_move.hpp>


void updateChecking(Board &b, ExtdMove* move)
{
    PieceType piece = static_cast<PieceType>(move->getPiece());
    Side s = b.curSide;
    Side enemy = b.curSide ^ 1;
    b.checkingSqs[enemy] = 0ULL;

    Square ksq = lsb(b.getUniquePiece(enemy, KING));

    b.checkingSqs[enemy] |= b.getUniquePiece(s, piece) & Attacks::getPieceAttacksRuntime(piece, ksq, b.occupancy, enemy);

    Alignment alligned = Between::getAlignment(ksq, move->getFrom());

    if(alligned == Alignment::NONE)
    {
        return;
    }
    else if(alligned == Alignment::STRAIGHT)
    {
        b.checkingSqs[enemy] |= Attacks::getRookAttacks(ksq, b.occupancy) & (b.getUniquePiece(b.curSide, ROOK) | b.getUniquePiece(b.curSide, QUEEN));
        return;
    }
    else if (alligned == Alignment::DIAGONAL) 
    {
        b.checkingSqs[enemy] |= Attacks::getBishopAttacks(ksq, b.occupancy) & (b.getUniquePiece(b.curSide, BISHOP) | b.getUniquePiece(b.curSide, QUEEN));
        return;
    }

}

void reviewCastlingPermissions(Board& b, Square from, Square to, Piece piece, Side s)
{
    if(piece == ROOK) 
    {
        Castling newCastlingPerm = b.curState.castlingRights;

        if(from == h1 || to == h1) { newCastlingPerm = newCastlingPerm & ~Castling::WK; }      // White kingside
        else if(from == a1 || to == a1) { newCastlingPerm = newCastlingPerm & ~Castling::WQ; } // White queenside  
        else if(from == h8 || to == h8) { newCastlingPerm = newCastlingPerm & ~Castling::BK; } // Black kingside
        else if(from == a8 || to == a8) { newCastlingPerm = newCastlingPerm & ~Castling::BQ; } // Black queenside
        
        updateCastlingPerm(b, newCastlingPerm);
        return;
    }

    if (piece == KING) 
    {
        updateCastlingPerm(b, b.curState.castlingRights & ~(s == WHITE ? Castling::WHITE : Castling::BLACK));
    }
}

void castlingMoveHelper(Board& b, Side s, Square kingFrom, Square kingTo)
{
    const bool isKingside = (kingTo > kingFrom);

    U8 rookFrom = noSquare;
    U8 rookTo = noSquare;

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

void afterMoveHelper(Board &b, ExtdMove* move)
{

    b.curState.halfmoveCount++;
    if(b.curSide == BLACK) { b.curState.fullmoveCount++; }
    if(move->getPiece() == PAWN || move->getCapture() != NONE) { b.curState.halfmoveCount = 0; }   
    
    reviewCastlingPermissions(b, move->getFrom(), move->getTo(), move->getPiece(), b.curSide);

    updateChecking(b, move);

    swapSides(b);
}

void doMove(Board &b, ExtdMove* move)
{
    Side s = b.curSide;
    Side enemy = b.curSide ^ 1;
    Square from = move->getFrom();
    Square to = move->getTo();
    Piece piece = move->getPiece();
    b.stateHistory[b.historyCount++] = b.curState;

    b.curState.epSq = EpSquare::NONE;

    if(move->getCapture() != NONE)
    {
        Piece captured = move->getCapture();
        removePiece(b, enemy, captured, to);
        b.curState.capturedPiece = captured;
    }
    
    if(move->getPromoted() != NONE)
    {
        removePiece(b, enemy, piece, from);
        putPiece(b, s, move->getPromoted(), to);

        afterMoveHelper(b, move);
        return;
    }

    if(move->isDouble())
    {
        b.curState.epSq = squareToEpsquare(to, s);
    }

    if(move->isCastle())
    {
        castlingMoveHelper(b, s, from, to);
    }

    movePiece(b, b.curSide, piece, from, to);
    afterMoveHelper(b, move);
}


void undoMove(Board &b, Move move)
{
    
}
