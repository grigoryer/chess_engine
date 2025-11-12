#pragma once
#include <constants.hpp>



constexpr int BISHOP_BLOCKER = 1024;
constexpr int ROOK_BLOCKER = 4096;
//max relevant occuapncy per piece 2^10 and 2^12


enum KnightDirection
{
    NE_LONG   = 17,
    NE_SHORT  = 10,
    NW_SHORT  = 6,
    NW_LONG   = 15,

    SE_LONG   = -NW_LONG,
    SE_SHORT  = -NW_SHORT,
    SW_SHORT  = -NE_SHORT,
    SW_LONG   = -NE_LONG,
};

template<KnightDirection D>
constexpr Bitboard shiftKnight(Bitboard bb) 
{
    if constexpr (D == NE_LONG)     return (bb & ~FileHBB) << 17;
    if constexpr (D == NE_SHORT)    return (bb & ~(FileGBB | FileHBB)) << 10;

    if constexpr (D == NW_LONG)     return (bb & ~FileABB) << 15;
    if constexpr (D == NW_SHORT)    return (bb & ~(FileABB | FileBBB)) << 6;

    if constexpr (D == SE_LONG)     return (bb & ~FileHBB) >> 15;
    if constexpr (D == SE_SHORT)    return (bb & ~(FileGBB | FileHBB)) >> 6;

    if constexpr (D == SW_LONG)     return (bb & ~FileABB) >> 17;
    if constexpr (D == SW_SHORT)    return (bb & ~(FileABB | FileBBB)) >> 10;
}


namespace Attacks 
{
    //inline attack tables, in c++17 compiler merges them into one instead of creating indivdual copies per c++ file
    inline std::array<std::array<Bitboard, NUM_SQUARES>, NUM_COLOR> pawnAttacks;
    inline std::array<Bitboard, NUM_SQUARES> knightAttacks;
    inline std::array<Bitboard, NUM_SQUARES> kingAttacks;
    inline std::array<std::array<Bitboard, ROOK_BLOCKER>, NUM_SQUARES> rookAttacks;
    inline std::array<std::array<Bitboard, BISHOP_BLOCKER>, NUM_SQUARES> bishopAttacks;
    
    void initializeAttacks(); 
    Bitboard getBishopAttacks(Square sq, Bitboard occ);
    Bitboard getRookAttacks(Square sq, Bitboard occ);
    Bitboard getQueenAttacks(Square sq, Bitboard occ);


    template<PieceType P>
    Bitboard getPieceAttacks(Square sq, Bitboard occ)
    {
        if constexpr (P == BISHOP)
        {
            return getBishopAttacks(sq, occ);
        }
        else if constexpr (P == ROOK)
        {
            return getRookAttacks(sq, occ);
        }
        else if constexpr (P == QUEEN)
        {
            return getQueenAttacks(sq, occ);
        }
        else if constexpr (P == KING)
        {
            return kingAttacks[sq];
        }
        else if constexpr (P == KNIGHT)
        {
            return knightAttacks[sq];
        }
    }
}
