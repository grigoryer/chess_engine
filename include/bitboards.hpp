#pragma once
#include <string>
#include <iostream>
#include "zobrist.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "moves.hpp"
#include <array>


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

inline int bits_in_bitboard(U64 bitboard) {
    return __builtin_popcountll(bitboard);  
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
    Move move;

    GameState();
    void clear();
    int enpassant_to_square(int enpassant_num);
};

class History 
{
    private:
        GameState list[MAX_MOVES];
        int count;

    public:
        History();
        void push(GameState game_state);
        std::optional<GameState> pop();
        void clear();
        size_t len();
        GameState& get_ref(size_t index);
};

class Board 
{
    public:
        U64 bb_pieces[NUM_SIDES][NUM_PIECES] = {0ULL};
        U64 bb_side[NUM_SIDES] = {0ULL};
        int piece_list[NUM_SQUARES] = {0};
        GameState game_state;
        History history;
        ZobristRandoms zobrist_randoms;
        Move move;
        AttackTables attack_tables;
        MoveList move_list;
        
    public:
        Board();

        void init_piece_list();
        void init_pieces_per_side_bitboard();
        void init();

        U64 init_zobrist_key();


        //support functions
        U64 occupancy();
        void reset_board();
        
        U8 us();
        U8 opponent();

        int king_sqaure(int side);
        U64& get_piece(int side, int Piece);
        U64& get_side(int side);

        void fen_parser(const std::string& fen);
        void load_fen(const std::string& fen);
        void print_piece_list();


        //move functions gen_moves.cpp
        void move_piece(int side, int piece, int from, int to);
        void remove_piece(int side, int piece, int square);
        void put_piece(int side, int piece, int square);
        void swap_sides();
        void update_castling_permissions(U8 permissions);
        void set_epsquare(int ep_square);
        void clear_epsquare();

        void make_move(const Move move);
        void unmake_move(const Move move);

        void gen_white_moves();
        void gen_black_moves();
        void gen_moves(int side);

        void gen_white_pawn_moves(U64 occupancy, U64 enemies);
        void gen_black_pawn_moves(U64 occupancy, U64 enemies);

        void gen_knight_moves(int side, U64 enemies);
        void gen_bishop_moves(int side, U64 occupancy, U64 enemies);
        void gen_rook_moves(int side, U64 occupancy, U64 enemies);
        void gen_queen_moves(int side, U64 occupancy, U64 enemies);
        void gen_king_moves(int side, U64 occupancy, U64 enemies);
        bool is_square_attacked(int square, int side); 
        
        void enpassant_support();
        void castling_support(int king_target);
        void double_support(int target);
        int promotion_support();
        void move_support(int side, int piece, int target);

        bool has_bishop_pair(int side);
        bool is_white_square(int square);
        bool material_to_force_checkmate(); // incomplete
        
};