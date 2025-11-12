#pragma once
#include <constants.hpp>


class State
{
public:
    EpSquare epSq = EP_NONE;
    Castling castlingRights = ALL_CASTLING;
};

class Board
{

public:

    //bitboards and piece mailbox
    std::array<Bitboard, NUM_PIECES> pieceBB;
    std::array<Bitboard, NUM_COLOR> colorBB;
    std::array<Piece, NUM_SQUARES> pieceList;
    Bitboard occupancy;
    
    State curState;
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