#pragma once
#include <array>
#include <constants.hpp>

inline Square stringToSquare(const std::string& sqStr) 
{
    if (sqStr.length() != 2) return Squares::noSquare;
    
    U8 file = sqStr[0] - 'a';
    U8 rank = sqStr[1] - '1';
    
    if (file > 7 || rank > 7) return Squares::noSquare;
    
    return rank * 8 + file;
}


class ExtdMove;
//Move class encoding moves into a single number
//Info needed: From square, To square, Capture, EnPassant, Castling, Promotion, Double push, Piece type

constexpr U8  TO_SHIFT        = 6;
constexpr U8  PIECE_SHIFT     = 12;  // 3 bits
constexpr U8  PROM_SHIFT      = 15;  // 3 bits
constexpr U8  CAPT_SHIFT      = 18;  // 3 bits
constexpr U8  DOUBLE_SHIFT    = 21;
constexpr U8  EP_SHIFT        = 22;
constexpr U8  CASTLE_SHIFT    = 23;

constexpr U32 FROM_MASK   = 0x3F;
constexpr U32 TO_MASK     = 0xFC0;
constexpr U32 PIECE_MASK  = 0x7000;    // 3 bits at 12–14
constexpr U32 PROM_MASK   = 0x38000;   // 3 bits at 15–17
constexpr U32 CAPT_MASK   = 0x1C0000;  // 3 bits at 18–20
constexpr U32 DOUBLE_MASK = 0x200000;
constexpr U32 EP_MASK     = 0x400000;
constexpr U32 CAST_MASK   = 0x800000;

class Move
{
    U32 data = 0;

public:

    inline Square getFrom() const      { return data & FROM_MASK; }
    inline Square getTo() const        { return (data & TO_MASK) >> TO_SHIFT; }
    inline Piece getPiece() const      { return (data & PIECE_MASK) >> PIECE_SHIFT; }
    inline Piece getPromoted() const   { return (data & PROM_MASK) >> PROM_SHIFT; }
    inline Piece getCapture() const    { return (data & CAPT_MASK) >> CAPT_SHIFT; }

    inline bool isDouble() const       { return data & DOUBLE_MASK; }
    inline bool isEnpassant() const    { return data & EP_MASK; }
    inline bool isCastle() const       { return data & CAST_MASK; }
    inline bool isPromoted() const     { return getPromoted() != NONE; }
    inline bool isMate() const         { return data == 0; }

    inline void setData(U32 move) { data = move; }
};

class ExtdMove : public Move
{
public: 

    U32 score = 0;

    void scoreMove();

    inline void setMove(Square from, Square to, Piece piece, Piece capture = NONE, Piece promotion = NONE,  bool doublePush = false, bool enpassant = false, bool castle = false)
    {
        setData((from | (to << TO_SHIFT) | (piece << PIECE_SHIFT) |
                (promotion << PROM_SHIFT) | (capture << CAPT_SHIFT) |
                (doublePush << DOUBLE_SHIFT) | (enpassant << EP_SHIFT) |
                (castle << CASTLE_SHIFT)));
    }
};


class MoveList
{
public:

    std::array<ExtdMove, 256> list;
};