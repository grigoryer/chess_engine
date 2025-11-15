#pragma once
#include <array>
#include <cstdint>
#include <iostream>
#include <string>

using Bitboard = uint64_t;
using Key = uint64_t;
using Square = int8_t;
using Side = int8_t;
using Piece = int8_t;
using Score = int16_t;
using U64 = uint64_t;
using U32 = uint32_t;
using U16 = uint16_t;
using U8 = uint8_t;

//constant numerations.
constexpr int NUM_SQUARES = 64;
constexpr int NUM_SIDES = 2;
constexpr int NUM_RANKS = 8;
constexpr int NUM_FILES = 8;
constexpr int NUM_PIECES = 6;
constexpr int NUM_CASTLING = 16;
constexpr int NUM_EPSQUARES = 17;

//array sizing
constexpr int MAX_MOVES = 128;
constexpr int MAX_HISTORY = 512;

// file and ranks
// rank 1-8 files A-H
constexpr Bitboard FullBB = 0xFFFFFFFFFFFFFFFFULL;
constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;
constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);

// Enumerations


//Piece type
enum PieceType : Piece
{
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE
};

//Piece mailbox list
enum UniquePiece : Piece
{
    K = 0, Q = 1, R = 2, B = 3, N = 4, P = 5, //white pieces
    k = 6, q = 7, r = 8, b = 9, n = 10, p = 11, noPiece = 12
};

enum Sides : Side
{
    WHITE, BLACK
};

enum Squares : Square
{
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8, noSquare
};

inline std::array<std::string, NUM_SQUARES> squareArray{
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8",
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

enum EpSquare : Square
{
    EP_WHITE_A, EP_WHITE_B, EP_WHITE_C, EP_WHITE_D,
    EP_WHITE_E, EP_WHITE_F, EP_WHITE_G, EP_WHITE_H,
    EP_BLACK_A, EP_BLACK_B, EP_BLACK_C, EP_BLACK_D,
    EP_BLACK_E, EP_BLACK_F, EP_BLACK_G, EP_BLACK_H,
    EP_NONE
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

template<Direction D>
constexpr Bitboard shift(Bitboard b) 
{
    if constexpr (D == NORTH)      return b << 8;
    if constexpr (D == SOUTH)      return b >> 8;

    if constexpr (D == WEST)       return (b & ~FileABB) >> 1;
    if constexpr (D == EAST)       return (b & ~FileHBB) << 1;

    if constexpr (D == NORTH_EAST) return (b & ~FileHBB) << 9;
    if constexpr (D == NORTH_WEST) return (b & ~FileABB) << 7;

    if constexpr (D == SOUTH_EAST) return (b & ~FileHBB) >> 7;
    if constexpr (D == SOUTH_WEST) return (b & ~FileABB) >> 9;
}

enum Castling : U8
{ 
    NO_CASTLING = 0, WK = 1, WQ = 2, BK = 4, BQ = 8, WHITE_CASTLING = 3, BLACK_CASTLING = 12, ALL_CASTLING = 15 
};

inline Castling operator|(Castling a, Castling b) {
    return Castling(static_cast<U8>(a) | static_cast<U8>(b));
}
inline Castling& operator|=(Castling& a, Castling b) {
    a = a | b;
    return a;
}

// Bitboard operations
constexpr bool getBit(const Bitboard &bb, Square sq) 
{
    return (bb & (1ULL << sq)) != 0;
}

constexpr void setBit(Bitboard &bb, Square sq) 
{
    bb |= (1ULL << sq);
}

constexpr Square lsb(Bitboard bb) 
{
    return __builtin_ctzll(bb);
}

constexpr void popBit(Bitboard &bb, Square sq)
{
    bb &= ~(1ULL << sq);
}

constexpr Square popLsb(Bitboard &bb) 
{
    Square sq = lsb(bb);
    bb &= ~(1ULL << sq);
    return sq;
}

constexpr int bitCount(Bitboard bb) 
{
    return __builtin_popcountll(bb);  
} 

constexpr bool moreThanOneBit(Bitboard bb) 
{
    return __builtin_popcountll(bb) > 1;
} 

constexpr Bitboard squareBb(Square sq)
{
    return (1ULL << sq);
}

//Square conversions
constexpr U8 squareRank(U8 sq)
{
    return sq / 8;
}

constexpr U8 squareFile(U8 sq)
{
    return sq % 8;
}

constexpr U8 epsquareToSquare(U8 epSq)
{
    if(epSq == EP_NONE) { return noSquare; }
    if(epSq < EP_BLACK_A) { return a3 + epSq; }
    if(epSq >= EP_BLACK_A) { return a6 + (epSq - EP_BLACK_A); }
    
    return noSquare;
}


constexpr U8 epsquareToCaptureSquare(U8 epSq)
{
    if(epSq == EP_NONE) { return noSquare; }
    if(epSq < EP_BLACK_A) { return a4 + epSq; }
    if(epSq >= EP_BLACK_A) { return a5 + (epSq - EP_BLACK_A); }
    
    return noSquare;
}


const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string CUSTOM_FEN = "4k3/p1Pppp1P/1pp2P2/6pp/8/pppP4/PPppppP1/5K1R w - - 0 1";

const std::string empty_board =  "8/PPPP3P/8/8/8/8/PPP2P2/8 w - - ";
const std::string start_position =  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string tricky_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string killer_position =  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const std::string cmk_position = "rnbk1b1r/pp3ppp/2p5/4q1B1/4n3/8/PPP2PPP/2KR1BNR b - - 1 10";


const std::string perft_2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
const std::string perft_3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
const std::string perft_4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
const std::string perft_5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
const std::string perft_6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";