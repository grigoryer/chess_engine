#pragma once
#include <constants.hpp>



namespace Between 
{

    inline Alignment getAlignment(Square s1, Square s2)
    {
        if (s1 == s2) { return Alignment::NONE; }
        
        U8 rank_diff = abs(squareRank(s1) - squareRank(s2));
        U8 file_diff = abs(squareFile(s1) - squareFile(s2));
        
        // Same rank or file = straight line
        if (rank_diff == 0 || file_diff == 0) { return Alignment::STRAIGHT; }
        
        // Equal rank and file difference = diagonal
        if (rank_diff == file_diff) { return Alignment::DIAGONAL; }
        
        return Alignment::NONE;
    }


    inline std::array<std::array<Bitboard, NUM_SQUARES>, NUM_SQUARES> betweenArray;
    inline std::array<std::array<Bitboard, NUM_SQUARES>, NUM_SQUARES> throughArray;

    void initializeBetween();


    inline Bitboard betweenBB(Square s1, Square s2)
    {
        return betweenArray[s1][s2];
    }

    inline Bitboard throughBB(Square s1, Square s2)
    {
        return throughArray[s1][s2];
    }
}