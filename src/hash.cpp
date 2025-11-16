#include "constants.hpp"
#include <hash.hpp>
#include <random>


namespace ZobristHashing
{

    std::array<std::array<std::array<Key,NUM_SQUARES>,NUM_PIECES>,NUM_SIDES> rndPieces;
    std::array<Key, NUM_CASTLING> rndCastling;
    std::array<Key, NUM_SIDES> rndSide;
    std::array<Key, NUM_EPSQUARES> rndEp;

    Key zobEnpassant(EpSquare epSq)
    {
        return rndEp[static_cast<int>(epSq)]; 
    }

    Key zobPiece(Side s, Piece piece, Square sq)
    {
        return rndPieces[s][piece][sq];
    }

    Key zobCastling(Castling castlingPerm) 
    {
        return rndCastling[static_cast<int>(castlingPerm)];
    }
        
    Key zobSide(Side s) 
    {
        return rndSide[s];
    }

    Key initailizeHash(Board &b) 
    {
        Key key = 0;

        for (Side s = 0; s < NUM_SIDES; s++) 
        {
            for (Piece piece = KING; piece <= PAWN; piece++) 
            {
                Bitboard bitboard = b.getUniquePiece(s, piece);
                
                while (bitboard != 0) 
                {
                    key ^= zobPiece(s, piece, popLsb(bitboard));
                }
            }
        }

        key ^= zobCastling(b.curState.castlingRights);
        key ^= zobSide(b.curSide);
        key ^= zobEnpassant(b.curState.epSq);
        return key;    
    }

    void initializeZobristRandoms()
    {
        std::mt19937_64 random(time(0));


        for(int i = 0; i < NUM_SIDES; ++i)
        {
            rndSide[i] = random();

            for(int j = 0; j < NUM_PIECES; ++j)
            {
                for(int k = 0; k < NUM_SQUARES; ++k)
                {
                    rndPieces[i][j][k] = random();
                }
            }
        }

        for(int i = 0; i < NUM_CASTLING; ++i)
        {
            rndCastling[i] = random();
        }

        for(int i = 0; i < NUM_EPSQUARES; ++i)
        {
            rndEp[i] = random();
        }
    }
}