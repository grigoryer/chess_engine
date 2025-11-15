#include <constants.hpp>



namespace Between 
{
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