#pragma once
#include <cassert>
#include <constants.hpp>
#include <moves.hpp>
#include <position.hpp>
#include <debug.hpp>
#include <attacks.hpp>
#include <between.hpp>


Bitboard generateBlockers(Board& b, Side s);
bool isLegal(ExtdMove* move, Board& b, Side s, Bitboard blockers);

template<MoveType Mt>
ExtdMove* genPromotion(ExtdMove* list, Board& b, Bitboard promoPawns, Side side, Bitboard target)
{
    if constexpr (Mt == CAPTURE)
    {
        while (promoPawns) 
        {
            Square sq = popLsb(promoPawns);
            Bitboard attacks = Attacks::pawnAttacks[side][sq] & target;
            while (attacks) 
            {
                Square to = popLsb(attacks);
                Piece captPiece = b.pieceList[to];
                for(Piece promo = QUEEN; promo <= KNIGHT; promo++)
                {
                    list->setMove(sq, to,PAWN, captPiece, promo);
                    list++;
                }
            }
        }
    }
    else if constexpr (Mt == QUIET)
    {
        Direction fromDirection = (side == WHITE ? SOUTH : NORTH);
        promoPawns = (side == WHITE ? promoPawns << 8 : promoPawns >> 8); //8 means is one up and one back
        promoPawns &= target;

        while (promoPawns) 
        {
            Square sq = popLsb(promoPawns);
            for(Piece promo = QUEEN; promo < KNIGHT; promo++)
            {
                list->setMove(sq + fromDirection, sq, PAWN, NONE, promo);
                list++;
            }
        }
    }
    return list;
}

template<Side S>
ExtdMove* genPawnsQuiet(ExtdMove* list, Board& b, Bitboard target, Bitboard pawns)
{
    Direction fromDirection;
    Bitboard doublePawns;

    if (S == WHITE)
    {
        pawns = shift<NORTH>(pawns) & ~b.occupancy;  // Check occupancy first
        doublePawns = shift<NORTH>(pawns & Rank3BB) & ~b.occupancy & target;  // Then occupancy, then target
        fromDirection = SOUTH;
    }
    else
    {
        pawns = shift<SOUTH>(pawns) & ~b.occupancy;  // Check occupancy first
        fromDirection = NORTH;
        doublePawns = shift<SOUTH>(pawns & Rank6BB) & ~b.occupancy & target;
    }

    pawns &= target;
    
    while(doublePawns)
    {
        Square sq = popLsb(doublePawns);
        list->setMove(sq + fromDirection + fromDirection, sq, PAWN, NONE, NONE, true);
        list++;
    }

    while(pawns)
    {
        Square sq = popLsb(pawns);
        list->setMove(sq + fromDirection, sq, PAWN);
        list++;
    }
    return list;
}

// Helper function to generate capturing pawn moves
template<Side S>
ExtdMove* genPawnsCapture(ExtdMove* list, Board& b, Bitboard target, Bitboard pawns)
{   
    //enpassant generation
    if(b.curState.epSq != EpSquare::NONE)
    {
        Square epLocation = epsquareToSquare(b.curState.epSq);
        Bitboard epAttackers = Attacks::pawnAttacks[S ^ 1][epLocation] & pawns;
        while(epAttackers) 
        {
            list->setMove(popLsb(epAttackers), epLocation, PAWN, NONE, NONE, false, true);
            list++;
        }
    }

    while(pawns)
    {
        Square sq = popLsb(pawns);
        Bitboard attacks = Attacks::pawnAttacks[S][sq] & target;
        while(attacks) 
        {
            Square to = popLsb(attacks);
            list->setMove(sq, to, PAWN, b.pieceList[to]);
            list++;
        }

    }
    return list;
}

// Main pawn generation function
template<MoveType Mt>
ExtdMove* genPawns(ExtdMove* list, Board& b, Side side, Bitboard target)
{
    Bitboard pawns = b.getUniquePiece(side, PAWN);
    Bitboard promotionRank = (side == WHITE ? Rank7BB : Rank2BB);
    Bitboard promoPawns = pawns & promotionRank;
    pawns &= ~promoPawns;

    if(promoPawns != 0)
    {
        list = genPromotion<Mt>(list, b, promoPawns, side, target);
    }

    if constexpr (Mt == QUIET)
    {
        if (side == WHITE) {
            list = genPawnsQuiet<WHITE>(list, b, target, pawns);
        } else {
            list = genPawnsQuiet<BLACK>(list, b, target, pawns);
        }
    }
    else if constexpr (Mt == CAPTURE)
    {
        if (side == WHITE) {
            list = genPawnsCapture<WHITE>(list, b, target, pawns);
        } else {
            list = genPawnsCapture<BLACK>(list, b, target, pawns);
        }
    }
    else if constexpr (Mt == EVASIONS)
    {
        if (side == WHITE) {
            list = genPawnsQuiet<WHITE>(list, b, target & ~b.occupancy, pawns);
            list = genPawnsCapture<WHITE>(list, b, target & b.getSide(side ^ 1), pawns);
        } else {
            list = genPawnsQuiet<BLACK>(list, b, target & ~b.occupancy, pawns);
            list = genPawnsCapture<BLACK>(list, b, target & b.getSide(side ^ 1), pawns);
        }
    }

    return list;
}

template<PieceType P, MoveType Mt>
ExtdMove* genPieceMoves(ExtdMove* list, Board& b, Side side, Bitboard target)
{
    Bitboard curPieces = b.getUniquePiece(side, P);
    while(curPieces)
    {
        Square from = popLsb(curPieces);
        Bitboard movesBB = Attacks::getPieceAttacks<P>(from, b.occupancy, side) & target;
        while(movesBB)
        {
            Square to = popLsb(movesBB);
            list->setMove(from, to, P, b.pieceList[to]);
            list++;
        }
    }
    return list;
}

inline ExtdMove* genCastle(ExtdMove* list, Castling castleRights, Bitboard occ, Side side)
{
    if(side == WHITE)
    {
        if(((Castling::WK & castleRights) == Castling::WK) && !getBit(occ, f1) && !getBit(occ, g1))
        {
            list->setMove(e1, g1, KING, NONE, NONE, false, false, true); 
            list++;
        }
        if(((Castling::WQ & castleRights) == Castling::WQ) && !getBit(occ, b1) && !getBit(occ, c1) && !getBit(occ, d1))
        { 
            list->setMove(e1, c1, KING, NONE, NONE, false, false, true); 
            list++;
        }
    }
    else
    {
        if(((Castling::BK & castleRights) == Castling::BK) && !getBit(occ, f8) && !getBit(occ, g8))
        { 
            list->setMove(e8, g8, KING, NONE, NONE, false, false, true); 
            list++;
        }
        if (((Castling::BQ & castleRights) == Castling::BQ) && !getBit(occ, b8) && !getBit(occ, c8) && !getBit(occ, d8))
        {
            list->setMove(e8, c8, KING, NONE, NONE, false, false, true); 
            list++;
        }
    }
    return list;
}

//psuedo legal moves, deosnt check for pin detection. This will be passed into a lgeal checker and it will also score and remove the bad moves. 
template<MoveType Mt>
ExtdMove* generateMoves(ExtdMove* list, Board& b, Side s)
{ 

    Bitboard target = 0;

    if constexpr (Mt == EVASIONS)
    {
        Bitboard kingTarget = FullBB & ~b.getSide(s);
        if(bitCount(b.checkingSqs[s]) > 1)
        {
            return (list = genPieceMoves<KING, Mt>(list, b,s, kingTarget));
        }
        else 
        {
            target = Between::betweenBB(lsb(b.checkingSqs[s]), lsb(b.getUniquePiece(s, KING))) | b.checkingSqs[s];
            list = genPieceMoves<KING, Mt>(list, b,s, kingTarget);
        }
    }
    else if constexpr (Mt == CAPTURE)
    {
        target = b.getSide(s ^ 1);
        list = genPieceMoves<KING, Mt>(list, b,s, target);
    }
    else if constexpr (Mt == QUIET)
    {
        target = ~b.occupancy;
        list = genPieceMoves<KING, Mt>(list, b,s, target);
        list = genCastle(list, b.curState.castlingRights, b.occupancy, s);
    }


    list = genPieceMoves<QUEEN, Mt>(list, b,s, target);
    list = genPieceMoves<KNIGHT, Mt>(list, b, s, target);
    list = genPieceMoves<BISHOP, Mt>(list, b,s, target);
    list = genPieceMoves<ROOK, Mt>(list, b,s, target);
    list = genPawns<Mt>(list, b, s, target);
    
    return list;
}



ExtdMove* generateLegals(ExtdMove* list, Board& b, Side s);