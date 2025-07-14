#include "bitboards.hpp"
#include "zobrist.hpp"
#include <random>
#include <ctime>


ZobristRandoms::ZobristRandoms()
{

    std::mt19937_64 rd(12345);


    for(int i = 0; i < NUM_SIDES; ++i)
    {
        rnd_sides[i] = rd();

        for(int j = 0; j < NUM_PIECES; ++j)
        {
            for(int k = 0; k < NUM_SQUARES; ++k)
            {
                rnd_pieces[i][j][k] = rd();
            }
        }
    }

    for(int i = 0; i < NUM_CASTLING; ++i)
    {
        rnd_castling[i] = rd();
    }

    for(int i = 0; i < NUM_EPSQUARES; ++i)
    {
        rnd_ep[i] = rd();
    }

}

