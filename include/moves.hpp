#pragma once
#include "types.hpp"
#include <array>

// Move struct with bit encoding
struct Move {
    U32 move;
    
    // Bit masks for extraction
    static const U32 SOURCE_MASK = 0x3F;        // 6 bits: 0-5
    static const U32 TARGET_MASK = 0xFC0;       // 6 bits: 6-11
    static const U32 PIECE_MASK = 0xF000;       // 4 bits: 12-15
    static const U32 PROMOTED_MASK = 0xF0000;   // 4 bits: 16-19
    static const U32 CAPTURE_MASK = 0x100000;   // 1 bit: 20
    static const U32 DOUBLE_MASK = 0x200000;    // 1 bit: 21
    static const U32 ENPASSANT_MASK = 0x400000; // 1 bit: 22
    static const U32 CASTLING_MASK = 0x800000;  // 1 bit: 23
    
    // Constructors
    Move();
    Move(U8 source, U8 target, U8 piece, U8 promoted = 0, 
         bool capture = false, bool double_push = false, 
         bool enpassant = false, bool castling = false);
    
    // Getters
    U8 get_source() const;
    U8 get_target() const;
    U8 get_piece() const;
    U8 get_promoted() const;
    bool is_capture() const;
    bool is_double() const;
    bool is_enpassant() const;
    bool is_castling() const;
    
    // Setters
    void set_source(U8 source);
    void set_target(U8 target);
    void set_piece(U8 piece);
    void set_promoted(U8 promoted);
    void set_capture(bool capture);
    void set_double(bool double_push);
    void set_enpassant(bool enpassant);
    void set_castling(bool castling);
    
    // Utility
    bool is_null() const;
    void clear();

    //other
    bool is_promotion() const;
    bool is_quiet() const;
    bool is_empty() const;

    // Comparison operators
    void print_move(U8 active_color) const;
    static std::string n_to_sq(U8 num);
    static std::string n_to_piece(U8 num, U8 active_color);
    U32 get_raw() const;
};

struct MoveList
{
    std::array<Move, MAX_HISTORY> moves = {};
    int count = 0;

    void add(Move move);
    void clear();
    bool is_move_empty(U16 index) const;
};