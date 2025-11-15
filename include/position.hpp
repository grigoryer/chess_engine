#pragma once
#include <array>
#include <constants.hpp>


class State
{
public:
    EpSquare epSq = EP_NONE;
    Castling castlingRights = ALL_CASTLING;

    std::array<std::array<Bitboard, NUM_PIECES>, NUM_SIDES> checkSqs;
    std::array<Bitboard, NUM_SIDES> checkingSqs;
};

class Board
{
public:
    //bitboards and piece mailbox
    std::array<Bitboard, NUM_PIECES> pieceBB;
    std::array<Bitboard, NUM_SIDES> sideBB;
    std::array<Piece, NUM_SQUARES> pieceList;
    Bitboard occupancy;

    State curState;
    Side curSide;
    void fenParser(const std::string& fen);
    void init();

    Board();
    Board(const std::string& fen);

    void setCheckSqs(Side s);
    bool isCheck(Side s);

    
    inline Bitboard getSide(Side s) const
    {
        return sideBB[s];
    }

    inline Bitboard getUniquePiece(Side s, Piece p) const
    {
        return pieceBB[p] & sideBB[s];
    }

};