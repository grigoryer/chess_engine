#include "bitboards.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include <array>

// bitboards for leaper piece not jumping across edge.
const U64 NOT_AFILE = 18374403900871474942ULL;
const U64 NOT_HFILE = 9187201950435737471ULL;
const U64 NOT_ABFILE = 18229723555195321596ULL;
const U64 NOT_GHFILE = 4557430888798830399ULL;

// knight attack shifts
static U8 const knight_nw_flat = 6;
static U8 const knight_ne_flat = 10;
static U8 const knight_nw_long = 15;
static U8 const knight_ne_long = 17;
static U8 const knight_sw_flat = 10;
static U8 const knight_se_flat = 6;
static U8 const knight_sw_long = 17;
static U8 const knight_se_long = 15;

// files and Rank represitation for walls for sliding pieces.
static const int e_wall = 7;
static const int w_wall = 0;
static const int n_wall = 7;
static const int s_wall = 0;


// magic numbers for indexing.
static const std::array<U64, NUM_SQUARES> rook_magics = 
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

static const std::array<U64, NUM_SQUARES> bishop_magics = 
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
static const std::array<U64, NUM_SQUARES> rook_shift = 
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

static const std::array<U64, NUM_SQUARES> bishop_shift = 
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


AttackTables::AttackTables() //NOLINT
{
    init_leaper_pieces();
    init_slider_pieces();
}

U64 AttackTables::gen_pawn_attacks(U8 side, U8 square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    if(side == WHITE)
    {
        // white pawn attacks
        attacks |= (bitboard & NOT_AFILE) << nw_shift;
        attacks |= (bitboard & NOT_HFILE) << ne_shift;
    }
    else 
    {
        // black pawns attacks
        attacks |= (bitboard & NOT_AFILE) >> sw_shift;
        attacks |= (bitboard & NOT_HFILE) >> se_shift;
    }

    return attacks;
}

U64 AttackTables::gen_knight_attacks(U8 square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;


    set_bit(bitboard, square);

    // north knight attacks
    attacks |= (bitboard & NOT_HFILE) << knight_ne_long;
    attacks |= (bitboard & NOT_AFILE) << knight_nw_long;
    attacks |= (bitboard & NOT_ABFILE) << knight_nw_flat;
    attacks |= (bitboard & NOT_GHFILE) << knight_ne_flat;

    // south knight attacks
    attacks |= (bitboard & NOT_AFILE) >> knight_sw_long;
    attacks |= (bitboard & NOT_HFILE) >> knight_se_long;
    attacks |= (bitboard & NOT_GHFILE) >> knight_se_flat;
    attacks |= (bitboard & NOT_ABFILE) >> knight_sw_flat;

    return attacks;
}

U64 AttackTables::gen_king_attacks(U8 square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    // east attacks
    attacks |= (bitboard & NOT_HFILE) << e_shift;
    attacks |= (bitboard & NOT_HFILE) << ne_shift;
    attacks |= (bitboard & NOT_HFILE) >> se_shift;
    // west attacks
    attacks |= (bitboard & NOT_AFILE) >> w_shift;
    attacks |= (bitboard & NOT_AFILE) >> sw_shift;
    attacks |= (bitboard & NOT_AFILE) << nw_shift;
    // north attack
    attacks |= bitboard << n_shift;

    // south attack
    attacks |= bitboard >> s_shift;

    return attacks;
}

void AttackTables::init_leaper_pieces()
{
    for(U8 square = 0; square < NUM_SQUARES; square++)
    {
        king_attacks[square] = gen_king_attacks(square);
        knight_attacks[square] = gen_knight_attacks(square);

        for(U8 side = 0; side < NUM_SIDES; side++)
        {
            pawn_attacks[side][square] = gen_pawn_attacks(side, square);
        }
    }
}

U64 AttackTables::mask_rook_attacks(U8 square) 
{
    U64 rook_mask = 0ULL;

    //rank file
    int rank = 0;
    int file = 0;

    int target_rank = square / BOARD_LENGTH;
    int target_file = square % BOARD_LENGTH;


    // generate the attack mask excluding walls rank and files as they do not restrict movement
    // north
    for(rank = target_rank + 1; rank < n_wall; rank++) {rook_mask |= 1ULL << (rank * BOARD_LENGTH + target_file);}
    // south
    for(rank = target_rank - 1; rank > s_wall ; rank--) {rook_mask |= 1ULL << (rank * BOARD_LENGTH + target_file);}
    // east
    for(file = target_file + 1; file < e_wall; file++) {rook_mask |= 1ULL << (target_rank * BOARD_LENGTH + file);}
    // west
    for(file = target_file - 1; file > w_wall; file--) {rook_mask |= 1ULL << (target_rank * BOARD_LENGTH + file);}

    return rook_mask;
}

U64 AttackTables::mask_bishop_attacks(U8 square) 
{
    U64 bishop_mask = 0ULL;

    // rank file
    int rank = 0;
    int file = 0;

    int target_rank = square / BOARD_LENGTH;
    int target_file = square % BOARD_LENGTH;

    // north east
    for(rank = target_rank + 1, file = target_file + 1; rank < n_wall && file < e_wall; rank++, file++) { bishop_mask |= 1ULL << (rank * BOARD_LENGTH + file); }
    // south west
    for(rank = target_rank - 1, file = target_file - 1; rank > s_wall && file > w_wall; rank--, file--) { bishop_mask |= 1ULL << (rank * BOARD_LENGTH + file); }
    // north west
    for(rank = target_rank + 1, file = target_file - 1; rank < n_wall && file > w_wall; rank++, file--) { bishop_mask |= 1ULL << (rank * BOARD_LENGTH + file); }
    // south east
    for(rank = target_rank - 1, file = target_file + 1; rank > s_wall && file < e_wall; rank--, file++) { bishop_mask |= 1ULL << (rank * BOARD_LENGTH + file); }

    return bishop_mask;
}

U64 AttackTables::gen_occupancy(U64 index, U64 attack_mask)
{
    U64 occupancy = 0ULL;
    U64 attack_copy = attack_mask; // we need mask later on in generating the magic index
    int bits_in_mask = bits_in_bitboard(attack_copy);

    for(int count = 0; count < bits_in_mask; count++)
    {
        U8 square = lsb(attack_copy);
        pop_bit(attack_copy, square);

        if((index & (1 << count)) != 0){
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

U64 AttackTables::gen_rook_attacks_onfly(U8 square, U64 occupancy)
{   
    U64 attacks = 0ULL;;
    int rank = 0;
    int file = 0;

    // current rank and file;
    int target_rank = square / BOARD_LENGTH;
    int target_file = square % BOARD_LENGTH;

    // generate rooks with all attacks possible and check if the square is occupied then stop, include edges since this is entire attack

    // north
    for(rank = target_rank + 1; rank < (n_wall + 1); rank++) 
    {
        attacks |= 1ULL << (rank * BOARD_LENGTH + target_file);
        if(((1ULL << (rank * BOARD_LENGTH + target_file)) & occupancy) != 0) { break; }
    }
    // south
    for(rank = target_rank - 1; rank > (s_wall - 1); rank--) 
    {
        attacks |= 1ULL << (rank * BOARD_LENGTH + target_file);
        if(((1ULL << (rank * BOARD_LENGTH + target_file)) & occupancy) != 0) { break; }
    }
    // east
    for(file = target_file + 1; file < (e_wall + 1); file++) 
    {
        attacks |= 1ULL << (target_rank * BOARD_LENGTH + file);
        if(((1ULL << (target_rank * BOARD_LENGTH + file)) & occupancy) != 0) { break; }
    }
    // west
    for(file = target_file - 1; file > (w_wall - 1); file--) 
    {
        attacks |= 1ULL << (target_rank * BOARD_LENGTH + file);
        if(((1ULL << (target_rank * BOARD_LENGTH + file)) & occupancy) != 0) { break; }
        
    }

    return attacks;
}

U64 AttackTables::gen_bishop_attacks_onfly(U8 square, U64 occupancy)
{
    U64 attacks = 0ULL;

    int rank = 0;
    int file = 0;

    int target_rank = square / BOARD_LENGTH;
    int target_file = square % BOARD_LENGTH;

    // north east
    for (rank = target_rank + 1, file = target_file + 1; rank <= n_wall && file <= e_wall; rank++, file++) 
    {
        attacks |= (1ULL << (rank * BOARD_LENGTH + file ));
        if ((occupancy & (1ULL << (rank * BOARD_LENGTH + file))) != 0) { break; }
    }
    // south west
    for (rank = target_rank - 1, file = target_file - 1; rank >= s_wall && file >= w_wall; rank--, file--) 
    {
        attacks |= (1ULL << (rank * BOARD_LENGTH + file ));
        if ((occupancy & (1ULL << (rank * BOARD_LENGTH + file))) != 0) { break; }
    }
    // north west
    for (rank = target_rank + 1, file = target_file - 1; rank <= n_wall && file >= w_wall; rank++, file--) 
    {
        attacks |= (1ULL << (rank * BOARD_LENGTH + file ));
        if ((occupancy & (1ULL << (rank * BOARD_LENGTH + file))) != 0) { break; }
    }
    // south east
    for (rank = target_rank - 1, file = target_file + 1; rank >= s_wall && file <= e_wall; rank--, file++) 
    {
        attacks |= (1ULL << (rank * BOARD_LENGTH + file ));
        if ((occupancy & (1ULL << (rank * BOARD_LENGTH + file))) != 0) { break; }
    }

    return attacks;
}

inline U64 AttackTables::get_rook_attacks(U8 square, U64 occupancy) 
{
    U64 mask = mask_rook_attacks(square);
    occupancy &= mask;
    U64 index = (occupancy * rook_magics[square]) >> rook_shift[square];
    return rook_attacks[square][index];
}

inline U64 AttackTables::get_bishop_attacks(U8 square, U64 occupancy) 
{
    U64 mask = mask_bishop_attacks(square);
    occupancy &= mask;
    U64 index = (occupancy * bishop_magics[square]) >> bishop_shift[square];
    return bishop_attacks[square][index];
}

U64 AttackTables::get_queen_attacks(U8 square, U64 occupancy) 
{
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}

void AttackTables::init_slider_pieces()
{
    for(U8 square = 0; square < NUM_SQUARES; square++)
    {
        U64 mask = mask_rook_attacks(square);
        int bits = bits_in_bitboard(mask);
    
        for(int i = 0; i < (1 << bits); i++)
        {
            U64 occupancy = gen_occupancy(i, mask);
            U64 attacks = gen_rook_attacks_onfly(square, occupancy);

            U64 index = (occupancy * rook_magics[square]) >> rook_shift[square];
            rook_attacks[square][index] = attacks;
        }    
    }


    for(U8 square = 0; square < NUM_SQUARES; square++)
    {
        U64 mask = mask_bishop_attacks(square);
        int bits = bits_in_bitboard(mask);

        for(int i = 0; i < (1 << bits); i++)
        {
            U64 occupancy = gen_occupancy(i, mask);
            U64 attacks = gen_bishop_attacks_onfly(square, occupancy);

            U64 index = (occupancy * bishop_magics[square]) >> bishop_shift[square];
            bishop_attacks[square][index] = attacks;
        }    
    }
}















