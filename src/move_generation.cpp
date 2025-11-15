#include "constants.hpp"
#include <debug.hpp>
#include "moves.hpp"
#include <between.hpp>
#include <iostream>
#include <move_generation.hpp>
#include <attacks.hpp>



bool isSquareAttacked(Board& b, Square sq, Side s)
{
    if (b.getUniquePiece(s ^ 1, KING) & Attacks::getPieceAttacks<KING>(sq, b.occupancy, s))     { return true; }
    if (b.getUniquePiece(s ^ 1, QUEEN) & Attacks::getPieceAttacks<QUEEN>(sq, b.occupancy, s))   { return true; }
    if (b.getUniquePiece(s ^ 1, ROOK) & Attacks::getPieceAttacks<ROOK>(sq, b.occupancy, s))     { return true; }
    if (b.getUniquePiece(s ^ 1, BISHOP) & Attacks::getPieceAttacks<BISHOP>(sq, b.occupancy, s)) { return true; }
    if (b.getUniquePiece(s ^ 1, KNIGHT) & Attacks::getPieceAttacks<KNIGHT>(sq, b.occupancy, s)) { return true; }
    if (b.getUniquePiece(s ^ 1, PAWN) & Attacks::getPieceAttacks<PAWN>(sq, b.occupancy, s))     { return true; }

    return false;
}

Bitboard generateBlockers(Board& b, Side s)
{
    Bitboard pinners = 0;
    Bitboard blockers = 0;
    Side enemy = s ^ 1;
    Square ksq = lsb(b.getUniquePiece(s, KING));

    pinners = (b.getUniquePiece(enemy, ROOK) | b.getUniquePiece(enemy, QUEEN)) & Attacks::getPieceAttacks<ROOK>(ksq, 0, s);

    while(pinners) 
    {
        Bitboard bb = (Between::betweenBB(ksq, popLsb(pinners)) & b.occupancy) & b.getSide(s);
        if(bitCount(bb) == 1)
        {
            blockers |= bb;
        }
    }

    pinners = (b.getUniquePiece(enemy, BISHOP) | b.getUniquePiece(enemy, QUEEN)) & Attacks::getPieceAttacks<BISHOP>(ksq, 0, s);
    while(pinners) 
    {
        Bitboard bb = (Between::betweenBB(ksq, popLsb(pinners)) & b.occupancy) & b.getSide(s);
        if(bitCount(bb) == 1)
        {
            blockers |= bb;
        }
    }
    return blockers;
}

bool isLegal(ExtdMove* move, Board& b, Side s, Bitboard blockers)
{
    Square ksq = lsb(b.getUniquePiece(s, KING));
    Bitboard blockerBB = 1ULL << move->getFrom() & blockers;
    Side enemy = s ^ 1;

    Square from = move->getFrom();
    Square to = move->getTo();

    if(move->isEnpassant())
    {
        Bitboard occ = b.occupancy;
        Square epCaptureSq = epsquareToCaptureSquare(b.curState.epSq);
        popBit(occ, move->getFrom());             // remove moving pawns
        popBit(occ,epCaptureSq);               // remove captured pawn  
        setBit(occ,move->getTo());              // add moving pawn to destination

        Bitboard bishop_attackers = b.getUniquePiece(enemy, BISHOP) | b.getUniquePiece(enemy, QUEEN);
        Bitboard rook_attackers = b.getUniquePiece(enemy, ROOK) | b.getUniquePiece(enemy, QUEEN);
        
        return (!(Attacks::getPieceAttacks<BISHOP>(ksq, occ, enemy) & bishop_attackers) 
                && !(Attacks::getPieceAttacks<ROOK>(ksq, occ, enemy) & rook_attackers));
    }

    if (move->isCastle()) 
    {
        Direction shift = (to > from ? EAST : WEST);
        Square cur = from;

        while(cur != to)
        {
            cur += shift; 
            if(isSquareAttacked(b,cur, s)) 
            { 
                return false; 
            }
        }
        return true;
    }

    if(move->getPiece() == KING)
    {
        return !isSquareAttacked(b,to,s);
    }

    if(blockerBB != 0) //if this piece is a blocker
    {   
        return (1ULL<< move->getTo() & Between::throughBB(ksq, lsb(blockerBB))) != 0; // check if its to path is on pat
    }

    return true;
};

ExtdMove* generateLegals(ExtdMove* list, Board& b, Side s)
{
    list = generateMoves<QUIET>(list, b, s);
    list = generateMoves<CAPTURE>(list, b, s);
    return list;
}