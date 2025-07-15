#pragma once
#include "types.hpp"

//define attack tables

class AttackTables {

private: 
    U64 pawn_attacks[NUM_SIDES][NUM_SQUARES];
    U64 knight_attacks[NUM_SQUARES];
    U64 king_attacks[NUM_SQUARES];

public: 
    U64 gen_pawn_attacks(int side, int square);
    U64 gen_knight_attacks(int square);
    U64 gen_king_attacks(int square);
    void init_leaper_pieces();

    AttackTables();
};
