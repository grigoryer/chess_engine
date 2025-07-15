#pragma once
#include <cstdint>

using U64 = uint64_t;
using U16 = uint16_t;
using I16 = int16_t;
using U8 = uint8_t;

const int NUM_SQUARES = 64;
const int NUM_SIDES = 2;
const int NUM_PIECES = 6;
const int NUM_CASTLING = 16;
const int NUM_EPSQUARES = 17;
const int MAX_MOVES = 2048;

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const U64 NOT_AFILE = 18374403900871474942ULL;
const U64 NOT_HFILE = 9187201950435737471ULL;
const U64 NOT_ABFILE = 18229723555195321596ULL;
const U64 NOT_GHFILE = 4557430888798830399ULL;

enum Side 
{ WHITE, BLACK };

enum Pieces 
{ KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

enum Castling
{ wk = 1, wq = 2, bk = 4, bq = 8 };

enum Square 
{
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8
};

enum PieceList { 
    NONE = 0,
    K = 1,
    Q = 2,
    R = 3,
    B = 4,
    N = 5,
    P = 6,
    k = 7,
    q = 8,
    r = 9,
    b = 10,
    n = 11,
    p = 12
};