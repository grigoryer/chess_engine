#pragma once
#include <array>
#include <constants.hpp>


class State
{
public:
    Key hash = 0;
    EpSquare epSq = EpSquare::NONE;
    Castling castlingRights = Castling::ALL;
    U16 halfmoveCount = 0;
    U16 fullmoveCount = 0;
    U8 phaseScore = 0;
    Piece capturedPiece = NONE;

    void resetState();
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

    std::array<Bitboard, NUM_SIDES> checkingSqs{0};
    std::array<Bitboard, NUM_SIDES> blockersBB{0};
    std::array<Square, NUM_SIDES> kingSq;


    std::array<State, MAX_HISTORY> stateHistory;
    int historyCount = 0;

    void fenParser(const std::string& fen);
    void init();
    void resetBoard(const std::string& fen);

    Board();
    Board(const std::string& fen);

    void setChecking(Side s);
    bool isCheck(Side s);
    bool isDraw();

    
    [[nodiscard]] inline Bitboard getSide(Side s) const
    {
        return sideBB[s];
    }

    [[nodiscard]] inline Bitboard getUniquePiece(Side s, Piece p) const
    {
        return pieceBB[p] & sideBB[s];
    }
};