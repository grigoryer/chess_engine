#pragma once
#include "types.hpp"
#include "bitboards.hpp"

//define attack tables


inline int bits_in_bitmap(U64 bitboard) {
    return __builtin_popcountll(bitboard);  
} 

class AttackTables 
{
    public: 
        //leaper pieces
        U64 pawn_attacks[NUM_SIDES][NUM_SQUARES];
        U64 knight_attacks[NUM_SQUARES];
        U64 king_attacks[NUM_SQUARES];
        U64 gen_pawn_attacks(int side, int square);
        U64 gen_knight_attacks(int square);
        U64 gen_king_attacks(int square);
        void init_leaper_pieces();

        //slider pieces
        U64 rook_attacks[NUM_SQUARES][4096];
        U64 bishop_attacks[NUM_SQUARES][1024];
        U64 mask_rook_attacks(int square);
        U64 mask_bishop_attacks(int square);
        U64 gen_occupancy(int index, U64 attack_mask);

        U64 gen_rook_attacks_onfly(int square, U64 occupancy);
        U64 gen_bishop_attacks_onfly(int square, U64 occupancy);
        void init_slider_pieces();
        



        void debug_magic_mismatch();
        void comprehensive_magic_test();

        U64 get_rook_attacks(int square, U64 occupancy);
        U64 get_bishop_attacks(int square, U64 occupancy);

        AttackTables();
};


struct Magic {
    U64* ptr;  // pointer to attack_table for each particular square
    U64 mask;  // to mask relevant squares of both lines (no outer squares)
    U64 magic; // magic 64-bit factor
    int shift; // shift right
 };


