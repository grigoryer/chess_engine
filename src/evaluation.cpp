#include "constants.hpp"
#include <evaluation.hpp>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE

constexpr Square flipSquare(Square sq)
{
    return sq ^ 56;
}

inline int Evaluation::scorePosition(Piece piece, Square sq)
{
    switch(piece)
    {
        case(KING) : { return KING_EVAL_MAP[flipSquare(sq)]; }
        case(ROOK) : { return ROOK_EVAL_MAP[flipSquare(sq)]; }
        case(PAWN) : { return PAWN_EVAL_MAP[flipSquare(sq)]; }
        case(QUEEN) : { return QUEEN_EVAL_MAP[sq]; }
        case(KNIGHT) : { return KNIGHT_EVAL_MAP[sq]; }
        case(BISHOP) : { return BISHOP_EVAL_MAP[sq]; }
    }
    return 0;
}

int Evaluation::evaluateBoard(Board& b)
{
    int score = 0;

    for(Piece piece = QUEEN; piece <= PAWN; piece++)
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        while(whiteBB)
        {
            score += PIECE_SCORES[piece];
            score += scorePosition(piece, popLsb(whiteBB));
        }
        while(blackBB)
        {
            score -= PIECE_SCORES[piece];
            score -= scorePosition(piece, popLsb(blackBB));
        }
    }

    return score;
}