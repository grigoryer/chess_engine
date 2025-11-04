#pragma once
#include <cstdint>
#include <iostream>

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
constexpr int NUM_COLOR = 2;
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
enum PieceList : Piece
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
    EVASIONS, QUIET, CAPTURES, SPECIALS
};

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

    if(epSq >= EP_BLACK_A) { return a6 + epSq; }
    else { return epSq; }
}


//debug functions


inline void printBoard(Bitboard bitboard)
{
    using std::cout;

    cout << "\n";
    for (int rank = 7; rank >= 0; --rank)
    {
        cout << rank + 1 << " "; 
        for (int file = 0; file < NUM_FILES; ++file)
        {
            Square square = rank * 8 + file;
            char c = getBit(bitboard, square) ? '1' : '.';
            cout << c << ' ';
        }
        cout << '\n';
    }
    cout << "  a b c d e f g h\n\n";
    cout << "#: " << bitboard << '\n';
}