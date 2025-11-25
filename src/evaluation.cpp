#include "constants.hpp"
#include <evaluation.hpp>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE


int Evaluation::evaluateBoard(Board& b)
{
    int score = 0;

    for(Piece piece = QUEEN; piece <= PAWN; piece++)
    {
        int pieceCount = bitCount(b.getUniquePiece(WHITE, piece)) - bitCount(b.getUniquePiece(BLACK, piece));
        score += PIECE_SCORES[piece] * pieceCount;
    }

    return score;
}