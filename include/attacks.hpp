#pragma once
#include "types.hpp"
#include <array>

//max relevant occuapncy per piece 2^10 and 2^12
const U16 BISHOP_BLOCKER = 1024;
const U16 ROOK_BLOCKER = 4096;

//define attack tables
class AttackTables 
{
    public:
        //store bitboards of possible attacks for each square.
        std::array<std::array<U64, NUM_SQUARES>, NUM_SIDES> pawn_attacks;
        std::array<U64, NUM_SQUARES> knight_attacks;
        std::array<U64, NUM_SQUARES> king_attacks;

        //store bitboard sof possible attack for each square, depedning on relevant occupancy
        std::array<std::array<U64, ROOK_BLOCKER>, NUM_SQUARES> rook_attacks;
        std::array<std::array<U64, BISHOP_BLOCKER>, NUM_SQUARES> bishop_attacks;
    public: 

        //leaper pieces generation
        static U64 gen_pawn_attacks(U8 side, U8 square);
        static U64 gen_knight_attacks(U8 square);
        static U64 gen_king_attacks(U8 square);
        void init_leaper_pieces();

        //slider pieces generation
        static U64 mask_rook_attacks(U8 square);
        static U64 mask_bishop_attacks(U8 square);
        static U64 gen_occupancy(U64 index, U64 attack_mask);
        static U64 gen_rook_attacks_onfly(U8 square, U64 occupancy);
        static U64 gen_bishop_attacks_onfly(U8 square, U64 occupancy);

        //get functions
        U64 get_rook_attacks(U8 square, U64 occupancy);
        U64 get_bishop_attacks(U8 square, U64 occupancy);
        U64 get_queen_attacks(U8 square, U64 occupancy);
        void init_slider_pieces();

        AttackTables();
};
