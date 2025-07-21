#pragma once
#include <cstdint>
#include <string>

using U64 = uint64_t;
using U32 = uint32_t;
using U16 = uint16_t;
using I16 = int16_t;
using U8 = uint8_t;
using I8 = int8_t;

const U8 NUM_SQUARES = 64;
const U8 NUM_SIDES = 2;
const U8 NUM_PIECES = 6;
const U8 NUM_CASTLING = 16;
const U8 NUM_EPSQUARES = 17;
const U16 MAX_MOVES = 2048;
const U8 BOARD_LENGTH = 8;
const U8 MAX_RANK = 7;
const U8 MAX_FILE = 7;
const U16 MAX_HISTORY = 256;

const U8 ep_rank_black = 6;
const U8 ep_rank_white = 3;


// attack shifts
const U8 nw_shift = 7;
const U8 ne_shift = 9;
const U8 se_shift = 7;
const U8 sw_shift = 9;
const U8 n_shift = 8;
const U8 s_shift = 8;
const U8 e_shift = 1;
const U8 w_shift = 1;

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string CUSTOM_FEN = "qrqrbnbn/PPPPPPPP/7K/8/8/7k/pppppppp/QRQRBNBN w KQkq - 0 1";

const std::string empty_board =  "8/8/8/8/8/8/8/8 w - - ";
const std::string start_position =  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string tricky_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string killer_position =  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
const std::string cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 10 9 ";

enum Ep_square : U8
{
    ep_white_a, ep_white_b, ep_white_c, ep_white_d,
    ep_white_e, ep_white_f, ep_white_g, ep_white_h,
    ep_black_a, ep_black_b, ep_black_c, ep_black_d,
    ep_black_e, ep_black_f, ep_black_g, ep_black_h,
    ep_none
};

enum Promotion : U8 
{
    no_promotion = 0, n_promotion = 1, b_promotion = 2, r_promotion = 4, q_promotion = 8, 
};

enum Side : U8
{ WHITE, BLACK };

enum Pieces :U8
{ KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

enum Castling : U8
{ no_castling = 0, wk = 1, wq = 2, bk = 4, bq = 8, white_castling = 3, black_castling = 12, all_castling = 15 };

enum Square :U8
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

enum PieceList : U8 
{
    NONE = 255, 
    K = 0, Q = 1, R = 2, B = 3, N = 4, P = 5,
    k = 6, q = 7, r = 8, b = 9, n = 10, p = 11
};

