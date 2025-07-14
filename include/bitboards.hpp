#pragma once
#include <string>
#include <iostream>
#include "zobrist.hpp"
#include "types.hpp"


inline bool get_bit(const U64 &bitboard, int s) 
{
    return bitboard & (1ULL << s);
}

inline void set_bit(U64 &bitboard, int s) 
{
    bitboard |= (1ULL << s);
}

inline void pop_bit(U64 &bitboard, int s) 
{
    bitboard &= ~(1ULL << s);
}

inline int lsb(U64 bitboard) 
{
    return __builtin_ctzll(bitboard);
}

void print_board(U64 bitboard);



struct GameState 
{
    U8 active_color;
    U8 castling;
    U8 half_move_clock;
    U8 en_passant;
    U16 fullmove_number;
    U64 zobrist_key;
    I16 phase_value;
    //Move next_move;

    GameState();
};

struct History 
{
    GameState list[MAX_MOVES];
    size_t count;

    History();
    void push(GameState game_state);
    std::optional<GameState> pop();
    void clear();
    size_t len();
    GameState& get_ref(size_t index);
    
};

struct Board 
{
    U64 bb_pieces[NUM_SIDES][NUM_PIECES] = {0ULL};
    U64 bb_side[NUM_SIDES] = {0ULL};
    int piece_list[NUM_SQUARES] = {0};
    GameState game_state;
    History history;
    ZobristRandoms zobrist_randoms;

    Board();

    void init_piece_list();
    U64 init_zobrist_key() const;
    void init_pieces_per_side_bitboard();
    U64 occupancy();

    
    void init();//not complete
};