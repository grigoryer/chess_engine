#include "constants.hpp"
#include <evaluation.hpp>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE
//all arrays are from own sides perspective so for uneven
constexpr Square flipSquare(Square sq)
{
    return static_cast<Square>(sq ^ 56);
}

inline Score Evaluation::scorePosition(Piece piece, Square sq, bool isLateGame)
{
    sq = flipSquare(sq);

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
    for (Piece piece = KING; piece <= PAWN; piece++) 
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        while (whiteBB)
        {
            positionScore += scorePosition(piece, popLsb(whiteBB), lateGame);
        }
        while (blackBB)
        {
            positionScore -= scorePosition(piece, popLsb(blackBB), lateGame);
        }
    }
    
    return pieceScore + positionScore;
}