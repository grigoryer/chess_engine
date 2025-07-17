#include "bitboards.hpp"
#include "attacks.hpp"

const U64 NOT_AFILE = 18374403900871474942ULL;
const U64 NOT_HFILE = 9187201950435737471ULL;
const U64 NOT_ABFILE = 18229723555195321596ULL;
const U64 NOT_GHFILE = 4557430888798830399ULL;


static const U64 rook_magics[NUM_SQUARES] = {
    0xA180022080400230ull, 0x0040100040022000ull, 0x0080088020001002ull, 0x0080080280841000ull,
    0x4200042010460008ull, 0x04800A0003040080ull, 0x0400110082041008ull, 0x008000A041000880ull,
    0x10138001A080C010ull, 0x0000804008200480ull, 0x00010011012000C0ull, 0x0022004128102200ull,
    0x000200081201200Cull, 0x202A001048460004ull, 0x0081000100420004ull, 0x4000800380004500ull,
    0x0000208002904001ull, 0x0090004040026008ull, 0x0208808010002001ull, 0x2002020020704940ull,
    0x8048010008110005ull, 0x6820808004002200ull, 0x0A80040008023011ull, 0x00B1460000811044ull,
    0x4204400080008EA0ull, 0xB002400180200184ull, 0x2020200080100380ull, 0x0010080080100080ull,
    0x2204080080800400ull, 0x0000A40080360080ull, 0x02040604002810B1ull, 0x008C218600004104ull,
    0x8180004000402000ull, 0x488C402000401001ull, 0x4018A00080801004ull, 0x1230002105001008ull,
    0x8904800800800400ull, 0x0042000C42003810ull, 0x008408110400B012ull, 0x0018086182000401ull,
    0x2240088020C28000ull, 0x001001201040C004ull, 0x0A02008010420020ull, 0x0010003009010060ull,
    0x0004008008008014ull, 0x0080020004008080ull, 0x0282020001008080ull, 0x50000181204A0004ull,
    0x48FFFE99FECFAA00ull, 0x48FFFE99FECFAA00ull, 0x497FFFADFF9C2E00ull, 0x613FFFDDFFCE9200ull,
    0xFFFFFFE9FFE7CE00ull, 0xFFFFFFF5FFF3E600ull, 0x0010301802830400ull, 0x510FFFF5F63C96A0ull,
    0xEBFFFFB9FF9FC526ull, 0x61FFFEDDFEEDAEAEull, 0x53BFFFEDFFDEB1A2ull, 0x127FFFB9FFDFB5F6ull,
    0x411FFFDDFFDBF4D6ull, 0x0801000804000603ull, 0x0003FFEF27EEBE74ull, 0x7645FFFECBFEA79Eull,
};

static const U64 bishop_magics[NUM_SQUARES] = {
    0xFFEDF9FD7CFCFFFFull, 0xFC0962854A77F576ull, 0x5822022042000000ull, 0x2CA804A100200020ull,
    0x0204042200000900ull, 0x2002121024000002ull, 0xFC0A66C64A7EF576ull, 0x7FFDFDFCBD79FFFFull,
    0xFC0846A64A34FFF6ull, 0xFC087A874A3CF7F6ull, 0x1001080204002100ull, 0x1810080489021800ull,
    0x0062040420010A00ull, 0x5028043004300020ull, 0xFC0864AE59B4FF76ull, 0x3C0860AF4B35FF76ull,
    0x73C01AF56CF4CFFBull, 0x41A01CFAD64AAFFCull, 0x040C0422080A0598ull, 0x4228020082004050ull,
    0x0200800400E00100ull, 0x020B001230021040ull, 0x7C0C028F5B34FF76ull, 0xFC0A028E5AB4DF76ull,
    0x0020208050A42180ull, 0x001004804B280200ull, 0x2048020024040010ull, 0x0102C04004010200ull,
    0x020408204C002010ull, 0x02411100020080C1ull, 0x102A008084042100ull, 0x0941030000A09846ull,
    0x0244100800400200ull, 0x4000901010080696ull, 0x0000280404180020ull, 0x0800042008240100ull,
    0x0220008400088020ull, 0x04020182000904C9ull, 0x0023010400020600ull, 0x0041040020110302ull,
    0xDCEFD9B54BFCC09Full, 0xF95FFA765AFD602Bull, 0x1401210240484800ull, 0x0022244208010080ull,
    0x1105040104000210ull, 0x2040088800C40081ull, 0x43FF9A5CF4CA0C01ull, 0x4BFFCD8E7C587601ull,
    0xFC0FF2865334F576ull, 0xFC0BF6CE5924F576ull, 0x80000B0401040402ull, 0x0020004821880A00ull,
    0x8200002022440100ull, 0x0009431801010068ull, 0xC3FFB7DC36CA8C89ull, 0xC3FF8A54F4CA2C89ull,
    0xFFFFFCFCFD79EDFFull, 0xFC0863FCCB147576ull, 0x040C000022013020ull, 0x2000104000420600ull,
    0x0400000260142410ull, 0x0800633408100500ull, 0xFC087E8E4BB2F736ull, 0x43FF9E4EF4CA2C89ull,
};

static const U64 rook_shift[NUM_SQUARES] = {
    52, 53, 53, 53, 53, 53, 53, 52, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    53, 54, 54, 54, 54, 54, 54, 53, 
    52, 53, 53, 53, 53, 53, 53, 52
};

static const U64 bishop_shift[NUM_SQUARES] = {
    58, 59, 59, 59, 59, 59, 59, 58, 
    59, 59, 59, 59, 59, 59, 59, 59, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 55, 55, 57, 59, 59, 
    59, 59, 57, 57, 57, 57, 59, 59, 
    59, 59, 59, 59, 59, 59, 59, 59, 
    58, 59, 59, 59, 59, 59, 59, 58
};

AttackTables::AttackTables()
{
    init_leaper_pieces();
    init_slider_pieces();
}

inline U64 AttackTables::gen_pawn_attacks(int side, int square)
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

inline U64 AttackTables::gen_knight_attacks(int square)
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

inline U64 AttackTables::gen_king_attacks(int square)
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
    U64 bishop_mask = 0ULL;

    //rank file
    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    //north east
    for(rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++) {bishop_mask |= 1ULL << (rank * 8 + file);}
    //south west
    for(rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--) {bishop_mask |= 1ULL << (rank * 8 + file);}
    //north west
    for(rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--) {bishop_mask |= 1ULL << (rank * 8 + file);}
    //south east
    for(rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++) {bishop_mask |= 1ULL << (rank * 8 + file);}

    return bishop_mask;
}

U64 AttackTables::gen_occupancy(int index, U64 attack_mask)
{
    U64 occupancy = 0ULL;
    U64 attack_copy = attack_mask;

    int bits_in_mask = bits_in_bitboard(attack_copy);

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

U64 AttackTables::gen_rook_attacks_onfly(int square, U64 occupancy)
{   
    U64 attacks = 0ULL;;

    //rank file
    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    //north
    for(rank = target_rank + 1; rank <= 7; rank++) 
    {
        attacks |= 1ULL << (rank * 8 + target_file);
        if((1ULL << (rank * 8 + target_file)) & occupancy)
        {
            break;
        }
    }
    //south
    for(rank = target_rank - 1; rank >= 0; rank--) 
    {
        attacks |= 1ULL << (rank * 8 + target_file);
        if((1ULL << (rank * 8 + target_file)) & occupancy)
        {
            break;
        }
    }
    //east
    for(file = target_file + 1; file <= 7; file++) 
    {
        attacks |= 1ULL << (target_rank * 8 + file);
        if((1ULL << (target_rank * 8 + file)) & occupancy)
        {
            break;
        }
    }
    //west
    for(file = target_file - 1; file >= 0; file--) 
    {
        attacks |= 1ULL << (target_rank * 8 + file);
        if((1ULL << (target_rank * 8 + file)) & occupancy)
        {
            break;
        }
    }

    return attacks;
    

}

U64 AttackTables::gen_bishop_attacks_onfly(int square, U64 occupancy)
{
    U64 attacks = 0ULL;

    //rank file
    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    //north east
    for(rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++) 
    {
        attacks |= 1ULL << (rank * 8 + file);
        if((1ULL << (rank * 8 + file)) & occupancy)
        {
            break;
        }
    }
    //south west
    for(rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) 
    {
        attacks |= 1ULL << (rank * 8 + file);
        if((1ULL << (rank * 8 + file)) & occupancy)
        {
            break;
        }
    }
    //north west
    for(rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) 
    {
        attacks |= 1ULL << (rank * 8 + file);
        if((1ULL << (rank * 8 + file)) & occupancy)
        {
            break;
        }
    }
    //south east
    for(rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) 
    {
        attacks |= 1ULL << (rank * 8 + file);
        if((1ULL << (rank * 8 + file)) & occupancy)
        {
            break;
        }
    }

    return attacks;
}

inline U64 AttackTables::get_rook_attacks(int square, U64 occupancy) 
{
    U64 mask = mask_rook_attacks(square);
    occupancy &= mask;
    int index = (occupancy * rook_magics[square]) >> rook_shift[square];
    return rook_attacks[square][index];
}

inline U64 AttackTables::get_bishop_attacks(int square, U64 occupancy) 
{
    U64 mask = mask_bishop_attacks(square);
    occupancy &= mask;
    int index = (occupancy * bishop_magics[square]) >> bishop_shift[square];
    return bishop_attacks[square][index];
}

U64 AttackTables::get_queen_attacks(int square, U64 occupancy) 
{
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}

void AttackTables::init_slider_pieces()
{
    for(int square = 0; square < NUM_SQUARES; square++)
    {
        U64 mask = mask_rook_attacks(square);
        int bits = bits_in_bitboard(mask);
        
        for(int i = 0; i < (1 << bits); i++)
        {
            U64 occupancy = gen_occupancy(i, mask);
            U64 attacks = gen_rook_attacks_onfly(square, occupancy);

            int index = (occupancy * rook_magics[square]) >> rook_shift[square];
            rook_attacks[square][index] = attacks;
        }    
    }

    for(int square = 0; square < NUM_SQUARES; square++)
    {
        U64 mask = mask_bishop_attacks(square);
        int bits = bits_in_bitboard(mask);

        for(int i = 0; i < (1 << bits); i++)
        {
            U64 occupancy = gen_occupancy(i, mask);
            U64 attacks = gen_bishop_attacks_onfly(square, occupancy);

            int index = (occupancy * bishop_magics[square]) >> bishop_shift[square];
            bishop_attacks[square][index] = attacks;
        }    
    }
}















