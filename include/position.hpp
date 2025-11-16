#pragma once
#include <array>
#include <constants.hpp>


class State
{
public:
    Key hash = 0;
    EpSquare epSq = EpSquare::NONE;
    Castling castlingRights = Castling::ALL;
    U16 halfmoveCount;
    U16 fullmoveCount;
};

class Board
{
public:
    //bitboards and piece mailbox
    std::array<Bitboard, NUM_PIECES> pieceBB;
    std::array<Bitboard, NUM_SIDES> sideBB;
    std::array<UniquePiece, NUM_SQUARES> pieceList;
    Bitboard occupancy;

    State curState;
    Side curSide;


    std::array<std::array<Bitboard, NUM_PIECES>, NUM_SIDES> checkSqs;
    std::array<Bitboard, NUM_SIDES> checkingSqs;


    std::array<State, MAX_HISTORY> stateHistory;
    int historyCount = 0;

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