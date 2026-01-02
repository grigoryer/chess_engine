#pragma once
#include <array>
#include <cstdint>
#include <string>

// ============================================================================
// TYPE ALIASES
// ============================================================================

using U8  = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

using Bitboard = U64;
using Key      = U64;
using Square   = int8_t;
using Side     = int8_t;
using Piece    = int8_t;
using Score    = int16_t;

// ============================================================================
// CONSTANTS
// ============================================================================

constexpr int NUM_SQUARES    = 64;
constexpr int NUM_SIDES      = 2;
constexpr int NUM_RANKS      = 8;
constexpr int NUM_FILES      = 8;
constexpr int NUM_PIECES     = 6;
constexpr int NUM_CASTLING   = 16;
constexpr int NUM_EPSQUARES  = 17;
constexpr int NUM__TYPE_SLIDERS = 2;

constexpr int MAX_MOVES      = 128;
constexpr int MAX_HISTORY    = 512;

// ============================================================================
// BITBOARD CONSTANTS
// ============================================================================

constexpr Bitboard FullBB = 0xFFFFFFFFFFFFFFFFULL;

// Files
constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;

// Ranks
constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);


// ============================================================================
// SEARCH CONSTANTS
// ============================================================================

const int TTABLE_SIZE = 5000000;

const int q_depth = 3;



// ============================================================================
// ENUMERATIONS
// ============================================================================


enum PieceType : Piece 
{
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE
};


const std::array<int, NUM_PIECES> PIECE_SCORES = {0, 900, 500, 315, 300, 100};

enum Sides : Side
{
    WHITE, BLACK
};

enum File : U8
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
};   

enum Rank : U8
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
};

enum Direction : int8_t
{
    NORTH = 8,
    EAST  = 1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

enum MoveType 
{
    QUIET, CAPTURE, LEGAL, EVASIONS
};

enum class Castling : U8 
{
    NONE  = 0,
    WK    = 1,
    WQ    = 2,
    BK    = 4,
    BQ    = 8,
    WHITE = 3,
    BLACK = 12,
    ALL = 15
};


enum class Alignment
{
    NONE,
    STRAIGHT,
    DIAGONAL
};


// Bitwise operations for Castling
inline Castling operator | (Castling a, Castling b) 
{
    return Castling(static_cast<U8>(a) | static_cast<U8>(b));
}

inline Castling& operator |= (Castling& a, Castling b) {
    a = a | b;
    return a;
}

inline Castling operator&(Castling a, Castling b) {
    return Castling(static_cast<U8>(a) & static_cast<U8>(b));
}

inline Castling& operator&=(Castling& a, Castling b) {
    a = a & b;
    return a;
}

inline Castling operator~(Castling a) {
    return Castling(~static_cast<U8>(a));
}

// ============================================================================
// SQUARE ENUMERATION AND LOOKUP
// ============================================================================

enum Squares : Square 
{
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8,
    noSquare
};

inline constexpr std::array<std::string_view, NUM_SQUARES> SQUARE_NAMES
{
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8",
};

enum class EpSquare : Square {
    WA, WB, WC, WD, WE, WF, WG, WH,
    BA, BB, BC, BD, BE, BF, BG, BH,
    NONE
};

// ============================================================================
// BITBOARD OPERATIONS
// ============================================================================

inline bool getBit(Bitboard bb, Square sq) 
{
    return (bb & (1ULL << sq)) != 0;
}

inline void setBit(Bitboard& bb, Square sq) 
{
    bb |= (1ULL << sq);
}

inline Square lsb(Bitboard bb) 
{
    return static_cast<Square>(__builtin_ctzll(bb));
}

inline void popBit(Bitboard& bb, Square sq) 
{
    bb &= ~(1ULL << sq);
}

inline Square popLsb(Bitboard& bb) 
{
    Square sq = lsb(bb);
    bb &= ~(1ULL << sq);
    return sq;
}

inline int bitCount(Bitboard bb) 
{
    return __builtin_popcountll(bb);
}

inline bool moreThanOneBit(Bitboard bb) 
{
    return __builtin_popcountll(bb) > 1;
}

inline Bitboard squareBb(Square sq) 
{
    return 1ULL << sq;
}

// ============================================================================
// SQUARE CONVERSIONS
// ============================================================================

inline U8 squareRank(Square sq) 
{
    return sq / 8;
}

inline U8 squareFile(Square sq) 
{
    return sq % 8;
}

//convert EPSQUARE to the TO square
inline Square epsquareToSquare(EpSquare epSq) 
{
    if (epSq == EpSquare::NONE) return static_cast<Square>(Squares::noSquare);
    if (static_cast<int>(epSq) < 8) {
        return static_cast<Square>(Squares::a3) + static_cast<int>(epSq);
    }
    return static_cast<Square>(Squares::a6) + (static_cast<int>(epSq) - 8);
}

//convert EPSQUARE to where the PAWN GETS CAPTURED.
inline Square epsquareToCaptureSquare(EpSquare epSq) 
{
    if (epSq == EpSquare::NONE) return static_cast<Square>(Squares::noSquare);
    if (static_cast<int>(epSq) < 8) {
        return static_cast<Square>(Squares::a4) + static_cast<int>(epSq);
    }
    return static_cast<Square>(Squares::a5) + (static_cast<int>(epSq) - 8);
}

//convert
inline Square squareToEPCaptureSquare(Square sq, Side s) 
{
    return sq + (s == WHITE ? -8 : 8);
}

inline EpSquare squareToEpsquare(Square sq, Side s) 
{
    return static_cast<EpSquare>(squareFile(sq) + (s == BLACK ? 8 : 0));
}

// ============================================================================
// BITBOARD SHIFT OPERATIONS
// ============================================================================

template<Direction D>
inline constexpr Bitboard shift(Bitboard b) 
{
    if constexpr (D == Direction::NORTH)      return b << 8;
    if constexpr (D == Direction::SOUTH)      return b >> 8;
    if constexpr (D == Direction::WEST)       return (b & ~FileABB) >> 1;
    if constexpr (D == Direction::EAST)       return (b & ~FileHBB) << 1;
    if constexpr (D == Direction::NORTH_EAST) return (b & ~FileHBB) << 9;
    if constexpr (D == Direction::NORTH_WEST) return (b & ~FileABB) << 7;
    if constexpr (D == Direction::SOUTH_EAST) return (b & ~FileHBB) >> 7;
    if constexpr (D == Direction::SOUTH_WEST) return (b & ~FileABB) >> 9;
}







const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string CUSTOM_FEN = "4k3/p1Pppp1P/1pp2P2/6pp/8/pppP4/PPppppP1/5K1R w - - 0 1";


const std::string startpos =  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string empty_board =  "8/PPPP3P/8/8/8/8/PPP2P2/8 w - - ";
const std::string tricky_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string killer_position =  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const std::string cmk_position = "rnbk1b1r/pp3ppp/2p5/4q1B1/4n3/8/PPP2PPP/2KR1BNR b - - 1 10";


const std::string perft_2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
const std::string perft_3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
const std::string perft_4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
const std::string perft_5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
const std::string perft_6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";