#include "types.hpp"
#include "bitboards.hpp"
#include "eval.hpp"


int evaluate(Board& board)
{
    int white_eval = count_material(WHITE, board);
    int black_eval = count_material(BLACK, board);

    int evaluation = white_eval - black_eval;

    int perspective = (board.game_state.active_color == WHITE) ? 1 : -1;

    return evaluation * perspective;
}

int count_material(U8 side, Board& board)
{
    int material = 0; 
    material += bits_in_bitboard(board.bb_pieces[side][PAWN]) * piece_values[PAWN];
    material += bits_in_bitboard(board.bb_pieces[side][KNIGHT]) * piece_values[KNIGHT];
    material += bits_in_bitboard(board.bb_pieces[side][BISHOP]) * piece_values[BISHOP];
    material += bits_in_bitboard(board.bb_pieces[side][ROOK]) * piece_values[ROOK];
    material += bits_in_bitboard(board.bb_pieces[side][QUEEN]) * piece_values[QUEEN];
    return material;
}