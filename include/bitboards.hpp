#pragma once
#include <string>
#include "zobrist.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "moves.hpp"
#include <array>


inline bool get_bit(const U64 &bitboard, U8 square) 
{
    return (bitboard & (1ULL << square)) != 0;
}

inline void set_bit(U64 &bitboard, U8 square) 
{
    bitboard |= (1ULL << square);
}

inline void pop_bit(U64 &bitboard, U8 square) 
{
    bitboard &= ~(1ULL << square);
}

inline U8 lsb(U64 bitboard) 
{
    return __builtin_ctzll(bitboard);
}

inline U8 bits_in_bitboard(U64 bitboard) {
    return __builtin_popcountll(bitboard);  
} 

void print_board(U64 bitboard);

struct GameState 
{
    U8 active_color = WHITE;
    U8 castling = all_castling;
    U8 half_move_clock = 0;
    U8 en_passant = ep_none;
    U16 fullmove_number = 1;
    U8 captured_piece = NONE;
    U64 zobrist_key = 0;
    Move move;

    GameState();
    void clear();
    void print_game_state();
};

class History 
{
    public:
        std::array<GameState, MAX_MOVES> list;
        U16 count = 0;

    public:
        History();
        void push(GameState game_state);
        void pop();
        void clear();
        U16 len();
        GameState top();

        GameState& get_ref(size_t index);
};

class Board 
{
    public:
        std::array<std::array<U64, NUM_PIECES>, NUM_SIDES> bb_pieces = {0ULL};
        std::array<U64, NUM_SIDES> bb_side = {0ULL};
        std::array<U8, NUM_SQUARES> piece_list = {NONE};

        GameState game_state;
        History history;
        ZobristRandoms zobrist_randoms;
        Move move;
        MoveList move_list;
        AttackTables attack_tables;
        
    public:
        Board();

        void init_piece_list();
        void init_pieces_per_side_bitboard();
        void init();
        U64 init_zobrist_key();

        U64 occupancy();
        void reset_board();
        U8 us();
        U8 opponent();
        U8 king_sqaure(U8 side);
        U64& get_piece(U8 side, U8 piece);
        U64& get_side(U8 side);
        void fen_parser(const std::string& fen);
        void load_fen(const std::string& fen);
        void print_piece_list();

        //zobrist updates
        void move_piece(U8 side, U8 piece, U8 from, U8 to);
        void remove_piece(U8 side, U8 piece, U8 square);
        void put_piece(U8 side, U8 piece, U8 square);
        void swap_sides();
        void update_castling_permissions(U8 permissions);
        void set_epsquare(U8 ep_square);
        void clear_epsquare();

        //generation
        void gen_white_moves();
        void gen_black_moves();
        void gen_moves(U8 side);
        void gen_white_pawn_moves(U64 occupancy, U64 enemies);
        void gen_black_pawn_moves(U64 occupancy, U64 enemies);
        void gen_knight_moves(U8 side, U64 enemies);
        void gen_bishop_moves(U8 side, U64 occupancy, U64 enemies);
        void gen_rook_moves(U8 side, U64 occupancy, U64 enemies);
        void gen_queen_moves(U8 side, U64 occupancy, U64 enemies);
        void gen_king_moves(U8 side, U64 occupancy, U64 enemies);
        void gen_castling_moves(U8 side, U64 occupancy);
        void try_white_kingside_castle(U64 occupancy);
        void try_white_queenside_castle(U64 occupancy);
        void try_black_kingside_castle(U64 occupancy);
        void try_black_queenside_castle(U64 occupancy);

        //make move
        void make_move(Move move);
        void castling_support(U8 side, U8 king_from, U8 king_to);
        void castling_permissions_support(U8 side, U8 from, U8 piece);
        void capture_support(U8 to);
        void ep_support(Move move, U8 side, U8 from, U8 to);
        void double_support(U8 side, U8 from, U8 to);
        void update_halfclock(bool is_capture, bool is_promotion, U8 piece);
        void promotion_support (const Move move, U8 side, U8 from, U8 to);

        //unmake move
        void unmake_move();
        void unmake_promotion(const Move& move, U8 side, U8 from, U8 to, const GameState& prev_state);
        void unmake_enpassant(U8 side, U8 from, U8 to, const GameState& prev_state);
        void unmake_castling(const Move& move);
        void unmake_capture(const Move& move, const GameState& prev_state);

        //utilities
        bool is_square_attacked(U8 square, U8 side); 
        bool is_check(U8 side);
        void under_check();
        bool has_bishop_pair(U8 side);
        static bool is_white_square(U8 square); 
        U8 piece_to_piecelist(U8 piece_list_type);
        U8 piecelist_to_piece(U8 piece);
        U8 enpassant_to_square(U8 enpassant_num);
};