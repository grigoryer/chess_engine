#pragma once
#include <constants.hpp>


class State
{



};



class Board
{

public:

    //bitboards and piece mailbox
    std::array<Bitboard, NUM_PIECES> pieceBB;
    std::array<Bitboard, NUM_COLOR> colorBB;
    std::array<Piece, NUM_SQUARES> pieceList;
    Bitboard occupancy;
    
    Side curSide;
    void fenParser(const std::string& fen);
    void init();



public:


    Board();
    Board(const std::string& fen);
    
    inline Bitboard getSide(Side s)
    {
        return colorBB[s];
    }

    inline Bitboard getUniquePiece(Side s, Piece p)
    {
        return pieceBB[p] & colorBB[s];
    }

};