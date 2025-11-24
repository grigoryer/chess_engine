#pragma once
#include <constants.hpp>



namespace Between 
{

    inline Alignment getAlignment(Square s1, Square s2)
    {
        if (s1 == s2) return Alignment::NONE;

        int df = (int(s1 & 7) - int(s2 & 7));     // file difference
        int dr = (int(s1 >> 3) - int(s2 >> 3));   // rank difference

        // Straight line: same file or rank
        if (df == 0 || dr == 0)
            return Alignment::STRAIGHT;

        // Diagonal line: |df| == |dr| without abs()
        if (df * df == dr * dr)
            return Alignment::DIAGONAL;

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