#include "constants.hpp"
#include <evaluation.hpp>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE
//all arrays are from own sides perspective, firs row = rank 1 for white and rank 8 for black

constexpr Square flipSquare(Square sq)
{
    return sq ^ 56;
}

inline Score Evaluation::scorePosition(Piece piece, Square sq, bool isLateGame)
{
    switch(piece)
    {
        case(KING) : { return isLateGame ? KING_LATE_EVAL_MAP[sq] : KING_EVAL_MAP[sq]; }
        case(PAWN) : { return isLateGame ? PAWN_LATE_EVAL_MAP[sq] : PAWN_EVAL_MAP[sq]; }
        case(ROOK) : { return ROOK_EVAL_MAP[sq]; }
        case(QUEEN) : { return QUEEN_EVAL_MAP[sq]; }
        case(KNIGHT) : { return KNIGHT_EVAL_MAP[sq]; }
        case(BISHOP) : { return BISHOP_EVAL_MAP[sq]; }
        default : {  }
    }
    return 0;
}

Score Evaluation::evaluateBoard(Board& b)
{
    Score pieceScore = 0;
    Score positionScore = 0;
    
    // calculate material first
    for (Piece piece = QUEEN; piece <= PAWN; piece++)
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        pieceScore +=  bitCount(whiteBB) * PIECE_SCORES[piece];
        pieceScore -= bitCount(blackBB) * PIECE_SCORES[piece];
    }
    
    bool lateGame = b.curState.phaseScore <= LATEGAME_PHASE;
    
    // evaluate non-pawn pieces normally
    for (Piece piece = QUEEN; piece <= PAWN; piece++) 
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        while (whiteBB)
        {
            Square sq = popLsb(whiteBB);
            positionScore += scorePosition(piece, sq, lateGame);
            
        }
        while (blackBB)
        {
            Square sq = popLsb(blackBB);
            sq = flipSquare(sq);
            positionScore -= scorePosition(piece, sq, lateGame);
        }
    }

    return (pieceScore + positionScore);
}