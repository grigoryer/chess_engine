#include "bitboards.hpp"
#include "moves.hpp"



const U64 NOT_AFILE = 18374403900871474942ULL;
const U64 NOT_HFILE = 9187201950435737471ULL;
const U64 NOT_ABFILE = 18229723555195321596ULL;
const U64 NOT_GHFILE = 4557430888798830399ULL;

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

void AttackTables::init_leaper_pieces()
{
    for(int square = 0; square < NUM_SQUARES; square++)
    {
        king_attacks[square] = gen_king_attacks(square);
        knight_attacks[square] = gen_knight_attacks(square);

        for(int side = 0; side < NUM_SIDES; side++)
        {
            pawn_attacks[side][square] = gen_pawn_attacks(side, square);
        }
    }
}

U64 AttackTables::mask_rook_attacks(int square) 
{
    U64 rook_mask = 0ULL;

    //rank file
    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    //north
    for(rank = target_rank + 1; rank <= 6; rank++) {rook_mask |= 1ULL << (rank * 8 + target_file);}
    //south
    for(rank = target_rank - 1; rank >= 1; rank--) {rook_mask |= 1ULL << (rank * 8 + target_file);}
    //east
    for(file = target_file + 1; file <= 6; file++) {rook_mask |= 1ULL << (target_rank * 8 + file);}
    //west
    for(file = target_file - 1; file >= 1; file--) {rook_mask |= 1ULL << (target_rank * 8 + file);}

    return rook_mask;
}

U64 AttackTables::mask_bishop_attacks(int square) 
{
    U64 rook_mask = 0ULL;

    //rank file
    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    std::cout << target_rank << std::endl;

    //north east
    for(rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++) {rook_mask |= 1ULL << (rank * 8 + file);}
    //south west
    for(rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--) {rook_mask |= 1ULL << (rank * 8 + file);}
    //north west
    for(rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--) {rook_mask |= 1ULL << (rank * 8 + file);}
    //south east
    for(rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++) {rook_mask |= 1ULL << (rank * 8 + file);}

    return rook_mask;
}

U64 AttackTables::gen_occupancy(int index, U64 attack_mask)
{
    U64 occupancy = 0ULL;
    U64 attack_copy = attack_mask;

    int bits_in_mask = __builtin_popcountll(attack_copy);

    for(int count = 0; count < bits_in_mask; count++)
    {
        int square = lsb(attack_copy);
        pop_bit(attack_copy, square);

        if(index & (1 << count)){
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}