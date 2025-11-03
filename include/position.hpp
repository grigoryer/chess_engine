#pragma once
#include <constants.hpp>


class State
{



};



class Board
{

private:

    //bitboards and piece mailbox
    std::array<Bitboard, NUM_PIECES> pieceBB;
    std::array<Bitboard, NUM_COLOR> colorBB;
    std::array<Piece, NUM_SQUARES> pieceListBoard;
    Bitboard occupancy;
    
    Side curSide;


public:




};