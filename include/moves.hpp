#pragma once
#include <constants.hpp>


//Move class encoding moves into a single number
//Info needed: From square, To square, Capture, EnPassant, Castling, Promotion, Double push, Piece type

constexpr U8 TO_SHIFT               = 6;
constexpr U8 PIECE_SHIFT            = 12;
constexpr U8 PROMOTED_SHIFT         = 16;
constexpr U8 CAPTURE_SHIFT          = 20;
constexpr U8 DOUBLE_SHIFT           = 21;
constexpr U8 ENPASSANT_SHIFT        = 22;
constexpr U8 CASTLE_SHIFT           = 23;

constexpr U32 FROM_MASK       = 0x3F;        // 6 bits
constexpr U32 TO_MASK         = 0xFC0;       // 6 bits (shifted by 6)
constexpr U32 PIECE_MASK      = 0xF000;      // 4 bits (shifted by 12)
constexpr U32 PROMOTED_MASK   = 0xF0000;     // 4 bits (shifted by 16)
constexpr U32 CAPTURE_MASK    = 0x100000;    // 1 bit
constexpr U32 DOUBLE_MASK     = 0x200000;    // 1 bit
constexpr U32 ENPASSANT_MASK  = 0x400000;    // 1 bit
constexpr U32 CASTLING_MASK   = 0x800000;    // 1 bit


class Move
{
    U32 data;

public:

    inline Square getFrom() const      { return data & FROM_MASK; }
    inline Square getTo() const        { return (data & TO_MASK) >> TO_SHIFT; }
    inline Piece getPiece() const      { return (data & PIECE_MASK) >> PIECE_SHIFT; }
    inline Piece getPromoted() const   { return (data & PROMOTED_MASK) >> PROMOTED_SHIFT; }

    inline bool isCapture() const      { return data & CAPTURE_MASK; }
    inline bool isDouble() const       { return data & DOUBLE_MASK; }
    inline bool isEnpassant() const    { return data & ENPASSANT_MASK; }
    inline bool isCastle() const       { return data & CASTLING_MASK; }
    inline bool isPromoted() const     { return getPromoted() != NONE; }

    inline void setData(U32 move) { data = move; }
};

class ExtdMove : public Move
{
    U32 score = 0;
    

public: 
    

    inline void setMove(Square from, Square to, Piece piece,bool capture = false, Piece promotion = NONE,  bool doublePush = false, bool enpassant = false, bool castle = false)
    {
        setData((from | (to << TO_SHIFT) | (piece << PIECE_SHIFT) |
                (promotion << PROMOTED_SHIFT) | (capture << CAPTURE_SHIFT) |
                (doublePush << DOUBLE_SHIFT) | (enpassant << ENPASSANT_SHIFT) |
                (castle << CASTLE_SHIFT)));
    }
};


class MoveList
{
public:

    std::array<ExtdMove, 256> list;


};