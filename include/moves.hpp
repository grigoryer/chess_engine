#pragma once
#include <constants.hpp>
#include <cstdint>


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

    Move(Square from, Square to, Piece piece);
    Move(Square from, Square to, Piece piece, bool capture);
    Move(Square from, Square to, Piece piece, bool capture, Piece promotion);
    Move(Square from, Square to, Piece piece, bool capture, Piece promotion, bool doublePush, bool enpassant, bool castle);

    inline Square getFrom()       { return data & FROM_MASK; }
    inline Square getTo()         { return (data & TO_MASK) >> TO_SHIFT; }
    inline Piece getPiece()       { return (data & PIECE_MASK) >> PIECE_SHIFT; }
    inline Piece getPromoted()    { return (data & PROMOTED_MASK) >> PROMOTED_SHIFT; }

    inline bool isCapture()       { return data & CAPTURE_MASK; }
    inline bool isDouble()        { return data & DOUBLE_MASK; }
    inline bool isEnpassant()     { return data & ENPASSANT_MASK; }
    inline bool isCastle()        { return data & CASTLING_MASK; }
    inline bool isPromoted()      { return getPromoted() != NONE; }
};

class ExtdMove : public Move
{
    U32 score;
};