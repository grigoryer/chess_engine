#include <cassert>
#include <constants.hpp>
#include <moves.hpp>
#include <position.hpp>
#include <moves.hpp>
#include <attacks.hpp>







static int count = 0;; 
template<MoveType Mt>
ExtdMove* genPromotion(ExtdMove* list, Bitboard promoPawns, Side side, Bitboard target)
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
                for(Piece promo = QUEEN; promo <= KNIGHT; promo++)
                {
                    list->setMove(sq, to,PAWN, true, promo);
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
            for(Piece promo = QUEEN; promo <= KNIGHT; promo++)
            {
                list->setMove(sq + fromDirection, sq, PAWN, false, promo);
                list++;
                
            }
        }
    }
    return list;
}

template<MoveType Mt>
ExtdMove* genPawns(ExtdMove* list, Board& b, Side side, Bitboard target)
{
    Bitboard pawns = b.getUniquePiece(side, PAWN);
    Bitboard promotionRank = (side == WHITE ? Rank7BB : Rank2BB);
    Bitboard promoPawns = pawns & promotionRank;
    pawns &= ~promoPawns;

    if(promoPawns != 0)
    {
        list = genPromotion<Mt>(list, promoPawns, side, target);
    }

    if constexpr (Mt == QUIET)
    {
        Direction fromDirection;
        Bitboard doublePawns;

        if (side == WHITE)
        {
            pawns = shift<NORTH>(pawns) & target;
            fromDirection = SOUTH;
            doublePawns = shift<NORTH>(pawns & Rank3BB) & target;
        }
        else
        {
            pawns = shift<SOUTH>(pawns) & target;
            fromDirection = NORTH;
            doublePawns = shift<SOUTH>(pawns & Rank6BB) & target;
        }

        while(doublePawns)
        {
            Square sq = popLsb(doublePawns);
            list->setMove(sq + fromDirection + fromDirection, sq, PAWN, false, NONE, true);
            list++;
        }

        while(pawns)
        {
            Square sq = popLsb(pawns);
            list->setMove(sq + fromDirection, sq, PAWN);
            list++;
        }
    }
    else if constexpr (Mt == CAPTURE)
    { 
        if(b.curState.epSq != EP_NONE)
        {
            Square epLocation = epsquareToSquare(b.curState.epSq);
            Bitboard epAttackers = Attacks::pawnAttacks[side ^ 1][epLocation] & pawns;
            while(epAttackers) 
            {
                list->setMove(popLsb(epAttackers), epLocation, PAWN, true, NONE, false, true);
                list++;
            }
        }

        while(pawns)
        {
            Square sq = popLsb(pawns);
            Bitboard attacks = Attacks::pawnAttacks[side][sq] & target;
            while(attacks) 
            {
                list->setMove(sq, popLsb(attacks), PAWN, true);
                list++;
            }
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
        Bitboard movesBB = Attacks::getPieceAttacks<P>(from, b.occupancy) & target;
        while(movesBB)
        {
            list->setMove(from, popLsb(movesBB), P, Mt == CAPTURE);
            list++;
        }
    }
    return list;
}

inline ExtdMove* genCastle(ExtdMove* list, Castling castleRights, Bitboard occ, Side side)
{
    if(side == WHITE)
    {
        if((WK & castleRights) == WK && !getBit(occ, f1) && !getBit(occ, g1))
        { 
            list->setMove(e1, g1, KING, false, NONE, false, false, true); 
            list++;
        }
        if ((WQ & castleRights) == WQ && !getBit(occ, b1) && !getBit(occ, c1) && !getBit(occ, d1))
        { 
            list->setMove(e1, c1, KING, false, NONE, false, false, true); 
            list++;
        }
    }
    else
    {
        if((BK & castleRights) == BK && !getBit(occ, f8) && !getBit(occ, g8))
        { 
            list->setMove(e8, g8, KING, false, NONE, false, false, true); 
            list++;
        }
        if ((BQ & castleRights) == BQ && !getBit(occ, b8) && !getBit(occ, c8) && !getBit(occ, d8))
        {
            list->setMove(e8, c8, KING, false, NONE, false, false, true); 
            list++;
        }
    }

    return list;
}

//psuedo legal moves, deosnt check for pin detection. This will be passed into a lgeal checker and it will also score and remove the bad moves. 

template<MoveType Mt>
ExtdMove* generateMoves(ExtdMove* list, Board& b)
{ 

    Bitboard target = 0;
    Side side = b.curSide;

    if constexpr (Mt == CAPTURE)
    {
        target = b.getSide(b.curSide ^ 1);
    }
    else if constexpr (Mt == QUIET)
    {
        target = ~b.occupancy;
        list = genCastle(list, b.curState.castlingRights, b.occupancy, side);
    }

    list = genPieceMoves<KNIGHT, Mt>(list, b, side, target);
    list = genPieceMoves<BISHOP, Mt>(list, b,side, target);
    list = genPieceMoves<ROOK, Mt>(list, b,side, target);
    list = genPieceMoves<QUEEN, Mt>(list, b,side, target);
    list = genPieceMoves<KING, Mt>(list, b,side, target);
    list = genPawns<Mt>(list, b, side, target);
    return list;
}
