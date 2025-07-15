#include "bitboards.hpp"
#include "moves.hpp"


AttackTables::AttackTables()
{
    init_leaper_pieces();
}

U64 AttackTables::gen_pawn_attacks(int side, int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    if(!side)
    {
        //white pawn attacks
        attacks |= (bitboard & NOT_AFILE) << 7;
        attacks |= (bitboard & NOT_HFILE) << 9;
    }
    else 
    {
        //black pawns attacks
        attacks |= (bitboard & NOT_AFILE) >> 9;
        attacks |= (bitboard & NOT_HFILE) >> 7;
    }

    return attacks;
}

U64 AttackTables::gen_knight_attacks(int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    //north knight attacks
    attacks |= (bitboard & NOT_HFILE) << 17;
    attacks |= (bitboard & NOT_AFILE) << 15;
    attacks |= (bitboard & NOT_ABFILE) << 6;
    attacks |= (bitboard & NOT_GHFILE) << 10;

    //south knight attacks
    attacks |= (bitboard & NOT_AFILE) >> 17;
    attacks |= (bitboard & NOT_HFILE) >> 15;
    attacks |= (bitboard & NOT_GHFILE) >> 6;
    attacks |= (bitboard & NOT_ABFILE) >> 10;

    return attacks;
}



U64 AttackTables::gen_king_attacks(int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    //king east attacks
    attacks |= (bitboard & NOT_HFILE) << 1;
    attacks |= (bitboard & NOT_HFILE) << 9;
    attacks |= (bitboard & NOT_HFILE) >> 7;

    //king west attacks
    attacks |= (bitboard & NOT_AFILE) >> 1;
    attacks |= (bitboard & NOT_AFILE) >> 9;
    attacks |= (bitboard & NOT_AFILE) << 7;

    //north attack
    attacks |= bitboard << 8;

    //south attack
    attacks |= bitboard >> 8;

    return attacks;
}


void AttackTables::init_leaper_pieces(){
    for(int square = 0; square < NUM_SQUARES; square++){
        king_attacks[square] = gen_king_attacks(square);
        knight_attacks[square] = gen_knight_attacks(square);

        for(int side = 0; side < NUM_SIDES; side++){
            pawn_attacks[side][square] = gen_pawn_attacks(side, square);
        }
    }
}