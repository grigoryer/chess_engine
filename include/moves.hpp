#pragma once
#include "types.hpp"

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
    Move(int source, int target, int piece, int promoted = 0, 
         bool capture = false, bool double_push = false, 
         bool enpassant = false, bool castling = false);
    
    // Getters
    int get_source() const;
    int get_target() const;
    int get_piece() const;
    int get_promoted() const;
    bool is_capture() const;
    bool is_double() const;
    bool is_enpassant() const;
    bool is_castling() const;
    
    // Setters
    void set_source(int source);
    void set_target(int target);
    void set_piece(int piece);
    void set_promoted(int promoted);
    void set_capture(bool capture);
    void set_double(bool double_push);
    void set_enpassant(bool enpassant);
    void set_castling(bool castling);
    
    // Utility
    bool is_null() const;
    void clear();
    
    // Comparison operators
    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;
    
    // For debugging/display
    U32 get_raw() const;
};