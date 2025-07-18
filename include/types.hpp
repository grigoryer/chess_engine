#pragma once

#include <cstdint>
#include <string>

using U64 = uint64_t;
using U32 = uint32_t;
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

const std::string CUSTOM_FEN = "r3kb1r/8/8/pP6/8/2Q5/8/R3KB1R w KQkq a6 0 1";

const std::string empty_board =  "8/8/8/8/8/8/8/8 w - - ";
const std::string start_position =  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string tricky_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string killer_position =  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const std::string cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

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
