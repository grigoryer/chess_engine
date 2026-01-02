#include "debug.hpp"
#include <cassert>
#include <constants.hpp>
#include <attacks.hpp>
#include <iostream>


//Leaper pieces generation
Bitboard genPawnAttacks(Side side, Square sq)
{
    Bitboard attacks = 0ULL;
    Bitboard bb = 0ULL;

    setBit(bb, sq);

    if(side == WHITE)
    {
        // white pawn attacks
        attacks |= shift<NORTH_WEST>(bb);
        attacks |= shift<NORTH_EAST>(bb);
    }
    else 
    {
        // black pawns attacks
        attacks |= shift<SOUTH_WEST>(bb);
        attacks |= shift<SOUTH_EAST>(bb);
    }

    return attacks;
}

Bitboard genKnightAttacks(Square sq)
{
    Bitboard attacks = 0ULL;
    Bitboard bb = 0ULL;


    setBit(bb, sq);

    // north knight attacks
    attacks |= shiftKnight<NE_LONG>(bb);
    attacks |= shiftKnight<NW_LONG>(bb);
    attacks |= shiftKnight<NE_SHORT>(bb);
    attacks |= shiftKnight<NW_SHORT>(bb);

    // south knight attacks
    attacks |= shiftKnight<SW_SHORT>(bb);
    attacks |= shiftKnight<SE_LONG>(bb);
    attacks |= shiftKnight<SW_LONG>(bb);
    attacks |= shiftKnight<SE_SHORT>(bb);

    return attacks;
}

Bitboard genKingAttacks(Square sq)
{
    Bitboard attacks = 0ULL;
    Bitboard bb = 0ULL;

    setBit(bb, sq);
    
    attacks |= shift<NORTH>(bb);
    attacks |= shift<SOUTH>(bb);

    attacks |= shift<WEST>(bb);
    attacks |= shift<EAST>(bb);

    attacks |= shift<NORTH_WEST>(bb);
    attacks |= shift<NORTH_EAST>(bb);

    attacks |= shift<SOUTH_WEST>(bb);
    attacks |= shift<SOUTH_EAST>(bb);

    return attacks;
}


//Slider piece generation
static constexpr std::array<Bitboard, NUM_SQUARES> rook_magics = 
{
    0xA180022080400230ULL, 0x0040100040022000ULL, 0x0080088020001002ULL, 0x0080080280841000ULL,
    0x4200042010460008ULL, 0x04800A0003040080ULL, 0x0400110082041008ULL, 0x008000A041000880ULL,
    0x10138001A080C010ULL, 0x0000804008200480ULL, 0x00010011012000C0ULL, 0x0022004128102200ULL,
    0x000200081201200CULL, 0x202A001048460004ULL, 0x0081000100420004ULL, 0x4000800380004500ULL,
    0x0000208002904001ULL, 0x0090004040026008ULL, 0x0208808010002001ULL, 0x2002020020704940ULL,
    0x8048010008110005ULL, 0x6820808004002200ULL, 0x0A80040008023011ULL, 0x00B1460000811044ULL,
    0x4204400080008EA0ULL, 0xB002400180200184ULL, 0x2020200080100380ULL, 0x0010080080100080ULL,
    0x2204080080800400ULL, 0x0000A40080360080ULL, 0x02040604002810B1ULL, 0x008C218600004104ULL,
    0x8180004000402000ULL, 0x488C402000401001ULL, 0x4018A00080801004ULL, 0x1230002105001008ULL,
    0x8904800800800400ULL, 0x0042000C42003810ULL, 0x008408110400B012ULL, 0x0018086182000401ULL,
    0x2240088020C28000ULL, 0x001001201040C004ULL, 0x0A02008010420020ULL, 0x0010003009010060ULL,
    0x0004008008008014ULL, 0x0080020004008080ULL, 0x0282020001008080ULL, 0x50000181204A0004ULL,
    0x48FFFE99FECFAA00ULL, 0x48FFFE99FECFAA00ULL, 0x497FFFADFF9C2E00ULL, 0x613FFFDDFFCE9200ULL,
    0xFFFFFFE9FFE7CE00ULL, 0xFFFFFFF5FFF3E600ULL, 0x0010301802830400ULL, 0x510FFFF5F63C96A0ULL,
    0xEBFFFFB9FF9FC526ULL, 0x61FFFEDDFEEDAEAEULL, 0x53BFFFEDFFDEB1A2ULL, 0x127FFFB9FFDFB5F6ULL,
    0x411FFFDDFFDBF4D6ULL, 0x0801000804000603ULL, 0x0003FFEF27EEBE74ULL, 0x7645FFFECBFEA79EULL,
};

static constexpr std::array<Bitboard, NUM_SQUARES> bishop_magics = 
{
    0xFFEDF9FD7CFCFFFFULL, 0xFC0962854A77F576ULL, 0x5822022042000000ULL, 0x2CA804A100200020ULL,
    0x0204042200000900ULL, 0x2002121024000002ULL, 0xFC0A66C64A7EF576ULL, 0x7FFDFDFCBD79FFFFULL,
    0xFC0846A64A34FFF6ULL, 0xFC087A874A3CF7F6ULL, 0x1001080204002100ULL, 0x1810080489021800ULL,
    0x0062040420010A00ULL, 0x5028043004300020ULL, 0xFC0864AE59B4FF76ULL, 0x3C0860AF4B35FF76ULL,
    0x73C01AF56CF4CFFBULL, 0x41A01CFAD64AAFFCULL, 0x040C0422080A0598ULL, 0x4228020082004050ULL,
    0x0200800400E00100ULL, 0x020B001230021040ULL, 0x7C0C028F5B34FF76ULL, 0xFC0A028E5AB4DF76ULL,
    0x0020208050A42180ULL, 0x001004804B280200ULL, 0x2048020024040010ULL, 0x0102C04004010200ULL,
    0x020408204C002010ULL, 0x02411100020080C1ULL, 0x102A008084042100ULL, 0x0941030000A09846ULL,
    0x0244100800400200ULL, 0x4000901010080696ULL, 0x0000280404180020ULL, 0x0800042008240100ULL,
    0x0220008400088020ULL, 0x04020182000904C9ULL, 0x0023010400020600ULL, 0x0041040020110302ULL,
    0xDCEFD9B54BFCC09FULL, 0xF95FFA765AFD602BULL, 0x1401210240484800ULL, 0x0022244208010080ULL,
    0x1105040104000210ULL, 0x2040088800C40081ULL, 0x43FF9A5CF4CA0C01ULL, 0x4BFFCD8E7C587601ULL,
    0xFC0FF2865334F576ULL, 0xFC0BF6CE5924F576ULL, 0x80000B0401040402ULL, 0x0020004821880A00ULL,
    0x8200002022440100ULL, 0x0009431801010068ULL, 0xC3FFB7DC36CA8C89ULL, 0xC3FF8A54F4CA2C89ULL,
    0xFFFFFCFCFD79EDFFULL, 0xFC0863FCCB147576ULL, 0x040C000022013020ULL, 0x2000104000420600ULL,
    0x0400000260142410ULL, 0x0800633408100500ULL, 0xFC087E8E4BB2F736ULL, 0x43FF9E4EF4CA2C89ULL,
};

// relevant occupancy bits depending on square. (64 - relevant occupancy bits)
static constexpr std::array<int, NUM_SQUARES> rook_shift = 
{
    52, 53, 53, 53, 53, 53, 53, 52, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    52, 53, 53, 53, 53, 53, 53, 52
};

static constexpr std::array<int, NUM_SQUARES> bishop_shift = 
{
    58, 59, 59, 59, 59, 59, 59, 58, 
    59, 59, 59, 59, 59, 59, 59, 59, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    59, 59, 59, 59, 59, 59, 59, 59, 
    58, 59, 59, 59, 59, 59, 59, 58
};
    
//create the masks for the sliding pieces, excluding edges. 
//We only need locations where a blocking piece would impact attack, meaning edges are excluded.
Bitboard maskRookAttacks(Square sq) 
{
    Bitboard rookMask = 0ULL;

    //rank file
    int rank = 0;
    int file = 0;

    int targetRank = sq / NUM_RANKS;
    int targetFile = sq % NUM_FILES;


    // generate the attack mask excluding walls rank and files as they do not restrict movement
    // north                                             
    for(rank = targetRank + 1; rank < RANK_8; rank++) { rookMask |= 1ULL << (rank * NUM_RANKS + targetFile); }
    // south
    for(rank = targetRank - 1; rank > RANK_1; rank--) { rookMask |= 1ULL << (rank * NUM_RANKS + targetFile); }
    // east
    for(file = targetFile + 1; file < FILE_H; file++) { rookMask |= 1ULL << (targetRank * NUM_RANKS + file); }
    // west
    for(file = targetFile - 1; file > FILE_A; file--) { rookMask |= 1ULL << (targetRank * NUM_RANKS + file); }

    return rookMask;
}

Bitboard maskBishopAttacks(Square sq) 
{
    Bitboard bishopMask = 0ULL;

    // rank file
    int rank = 0;
    int file = 0;

    int targetRank = sq / NUM_RANKS;
    int targetFile = sq % NUM_FILES;

    // north east
    for(rank = targetRank + 1, file = targetFile + 1; rank < RANK_8 && file < FILE_H; rank++, file++) { bishopMask |= 1ULL << (rank * NUM_RANKS + file); }
    // south west
    for(rank = targetRank - 1, file = targetFile - 1; rank > RANK_1 && file > FILE_A; rank--, file--) { bishopMask |= 1ULL << (rank * NUM_RANKS + file); }
    // north west
    for(rank = targetRank + 1, file = targetFile - 1; rank < RANK_8 && file > FILE_A; rank++, file--) { bishopMask |= 1ULL << (rank * NUM_RANKS + file); }
    // south east
    for(rank = targetRank - 1, file = targetFile + 1; rank > RANK_1 && file < FILE_H; rank--, file++) { bishopMask |= 1ULL << (rank * NUM_RANKS + file); }

    return bishopMask;
}

/*
generates occupancy based on index of that square unqiue occupancies of (2^10 for bishops and 2^12 for rooks).
index represents which bits are flipped on. if index = 15 bits 1,2,4,8 are on. that means the first 4 bits are occupied
Run through possible occupied bits, from bit mask, so if bishop is on a1 that is 6 possible bits. 
Then check if this index has this bit flipped on or of, if on add this to our occupancy.

Bascailly add the lsb bit location. (5 lsb = 2^5) and push the 5th lsb in the attack mask to on.
*/
Bitboard genOccupancy(int index, Bitboard attackMask)
{
    //blank occupancy map
    Bitboard occupancy = 0ULL;
    
    //total amount of bits added 2^bitCount in mask
    int bitCountMask = bitCount(attackMask);

    //for each relevant bit check if it is with the index.
    for(int count = 0; count < bitCountMask; count++)
    {
        //find square of bit and pop it.
        Square sq = popLsb(attackMask);

        //if bit is flipped on in this index add it to our occuapncy lsb.
        if((index & (1 << count)) != 0)
        {
            occupancy |= (1ULL << sq);
        }
    }
    return occupancy;
}

// Generate attack bitboard based on occupancy. Includes edges since this is entire attack.
// For each ray, (N,E,S,W) if it hits an occupied square you break stop since rest is blocked.
Bitboard genRookAttacksOnfly(Square sq, Bitboard occ)
{   
    Bitboard attacks = 0ULL;;
    int rank = 0;
    int file = 0;
    
    // current rank and file;
    int targetRank = sq / NUM_RANKS;
    int targetFile = sq % NUM_FILES;

    // north
    for(rank = targetRank + 1; rank <= RANK_8; rank++) 
    {
        attacks |= 1ULL << (rank * NUM_RANKS + targetFile);
        if(((1ULL << (rank * NUM_RANKS + targetFile)) & occ)) { break; }
    }
    // south
    for(rank = targetRank - 1; rank >= RANK_1; rank--) 
    {
        attacks |= 1ULL << (rank * NUM_RANKS + targetFile);
        if(((1ULL << (rank * NUM_RANKS + targetFile)) & occ) != 0) { break; }
    }
    // east
    for(file = targetFile + 1; file <= FILE_H; file++) 
    {
        attacks |= 1ULL << (targetRank * NUM_RANKS + file);
        if(((1ULL << (targetRank * NUM_RANKS + file)) & occ)) { break; }
    }
    // west
    for(file = targetFile - 1; file >= (FILE_A); file--) 
    {
        attacks |= 1ULL << (targetRank * NUM_RANKS + file);
        if(((1ULL << (targetRank * NUM_RANKS + file)) & occ) != 0) { break; }
        
    }
    return attacks;
}

// For each ray, (NW,NE,SE,SW) if it hits an occupied square you break stop since rest is blocked.
Bitboard genBishopAttacksOnfly(Square sq, Bitboard occ)
{
    Bitboard attacks = 0ULL;

    int rank = 0;
    int file = 0;

    int targetRank = sq / NUM_RANKS;
    int targetFile = sq % NUM_FILES;

    // north east
    for (rank = targetRank + 1, file = targetFile + 1; rank <= RANK_8 && file <= FILE_H; rank++, file++) 
    {
        attacks |= (1ULL << (rank * NUM_RANKS + file ));
        if ((occ & (1ULL << (rank * NUM_RANKS + file))) != 0) { break; }
    }
    // south west
    for (rank = targetRank - 1, file = targetFile - 1; rank >= RANK_1 && file >= FILE_A; rank--, file--) 
    {
        attacks |= (1ULL << (rank * NUM_RANKS + file ));
        if ((occ & (1ULL << (rank * NUM_RANKS + file))) != 0) { break; }
    }
    // north west
    for (rank = targetRank + 1, file = targetFile - 1; rank <= RANK_8 && file >= FILE_A; rank++, file--) 
    {
        attacks |= (1ULL << (rank * NUM_RANKS + file ));
        if ((occ & (1ULL << (rank * NUM_RANKS + file))) != 0) { break; }
    }
    // south east
    for (rank = targetRank - 1, file = targetFile + 1; rank >= RANK_1 && file <= FILE_H; rank--, file++) 
    {
        attacks |= (1ULL << (rank * NUM_RANKS + file ));
        if ((occ & (1ULL << (rank * NUM_RANKS + file))) != 0) { break; }
    }

    return attacks;
}






namespace Attacks
{
    Bitboard getRookAttacks(Square sq, Bitboard occ)
    {  
        occ &= maskRookAttacks(sq);  
        int index = (occ * rook_magics[sq]) >> rook_shift[sq]; 
        return rookAttacks[sq][index];  
    }  

    Bitboard getBishopAttacks(Square sq, Bitboard occ)
    {    
        occ &= maskBishopAttacks(sq);  
        int index = (occ * bishop_magics[sq]) >> bishop_shift[sq];  
        return bishopAttacks[sq][index];  
    }  

    Bitboard getQueenAttacks(Square sq, Bitboard occ)
    {    
        return getBishopAttacks(sq, occ) | getRookAttacks(sq, occ);
    }  

    //rooks bishop
    void initSliders()
    {
        for(Square square = 0; square < NUM_SQUARES; square++)
        {
            Bitboard mask = maskRookAttacks(square);
            int bits = bitCount(mask);
        
            for(int i = 0; i < (1 << bits); i++)
            {
                Bitboard occupancy = genOccupancy(i, mask);
                Bitboard attacks = genRookAttacksOnfly(square, occupancy);

                int index = (occupancy * rook_magics[square]) >> rook_shift[square];
                rookAttacks[square][index] = attacks;
            }    
        }


        for(Square square = 0; square < NUM_SQUARES; square++)
        {
            Bitboard mask = maskBishopAttacks(square);
            int bits = bitCount(mask);

            for(int i = 0; i < (1 << bits); i++)
            {
                Bitboard occupancy = genOccupancy(i, mask);
                Bitboard attacks = genBishopAttacksOnfly(square, occupancy);

                Bitboard index = (occupancy * bishop_magics[square]) >> bishop_shift[square];
                bishopAttacks[square][index] = attacks;
            }    
        }
    }

    //pawn king knight
    void initLeapers()
    {
        for(int sq = 0; sq < NUM_SQUARES; sq++)
        {
            kingAttacks[sq] = genKingAttacks(sq);
            knightAttacks[sq] = genKnightAttacks(sq);

            for(int side = 0; side < NUM_SIDES; side++)
            {
                pawnAttacks[side][sq] = genPawnAttacks(side, sq);
            }
        }
    }


    void initializeAttacks()
    {
        initSliders();
        initLeapers();
    }
}

