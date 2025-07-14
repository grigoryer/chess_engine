#pragma once
#include <string>
#include <iostream>

using U64 = uint64_t;
using U16 = uint16_t;
using I16 = int16_t;
using U8 = uint8_t;

const int NUM_SQUARES = 64;
const int NUM_SIDES = 2;
const int NUM_PIECES = 6;
const int NUM_CASTLING = 16;
const int NUM_EPSQUARES = 17;

enum Square {
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8,
};

enum Pieces {
    NONE = 0, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN 
};

enum Side {
    WHITE, BLACK
};

inline bool get_bit(const U64 &bitboard, int s) {
    return bitboard & (1ULL << s);
}

inline void set_bit(U64 &bitboard, int s) {
    bitboard |= (1ULL << s);
}

inline void pop_bit(U64 &bitboard, int s) {
    bitboard &= ~(1ULL << s);
}

inline int lsb(U64 bitboard) {
    return __builtin_ctzll(bitboard);
}


void print_board(U64 bitboard);


struct Board 
{
    U64 bb_pieces[NUM_SIDES][NUM_PIECES] = {0ULL};
    U64 bb_side[NUM_SIDES] = {0ULL};
};

struct GameState 
{
    Board board;
    U8 active_color;
    U8 castling;
    U8 half_move_clock;
    U8 en_passant;
    U16 fullmove_number;
    U64 zobrist_key;
    I16 phase_value;
    //Move next_move;

    U64 init_zobrist_key() const;
};