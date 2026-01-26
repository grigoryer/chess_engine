#include "constants.hpp"
#include <evaluation.hpp>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE


//all arrays are from own sides perspective so for uneven
constexpr Square flipSquare(Square sq)
{
    return sq ^ 56;
}

int Evaluation::pawnEvalFile(Board& b, U8 file, Side side, Bitboard pawns, Bitboard pawnsEnemy)
{
    int score = 0;
    Bitboard fileMask = FileABB << file;
    Bitboard pawnsOnFile = pawns & fileMask;
    
    // get most advanced pawn on this file for position bonus
    Square mostAdvanced = side == WHITE ? lsb(pawnsOnFile) : popLsb(pawnsOnFile);  // MSB for black
    score += PAWN_EVAL_MAP[flipSquare(mostAdvanced)];
    
    // doubled pawns penalty
    int pawnCount = bitCount(pawnsOnFile);
    score -= (pawnCount - 1) * 10;
    
    // passed pawn bonus
    if ((pawnsEnemy & fileMask) == 0)
    {
        score += 40;
    }
    
    // pawn chain bonus (friendly pawns on adjacent files)
    if (file > 0 && (pawns & (FileABB << (file - 1))) != 0)
    {
        score += 20;
    }
    if (file < 7 && (pawns & (FileABB << (file + 1))) != 0)
    {
        score += 20;
    }
    
    return score;
}

inline int Evaluation::scorePosition(Piece piece, Square sq, bool isLateGame)
{
    sq = flipSquare(sq);
    switch(piece)
    {
        case(KING) : { return isLateGame ? KING_LATE_EVAL_MAP[sq] : KING_EVAL_MAP[sq]; }
        case(ROOK) : { return ROOK_EVAL_MAP[sq]; }
        case(QUEEN) : { return QUEEN_EVAL_MAP[sq]; }
        case(KNIGHT) : { return KNIGHT_EVAL_MAP[sq]; }
        case(BISHOP) : { return BISHOP_EVAL_MAP[sq]; }
    }
    return 0;
}

int Evaluation::evaluateBoard(Board& b)
{
    int pieceScore = 0;
    int positionScore = 0;
    
    // calculate material first
    for (Piece piece = QUEEN; piece <= PAWN; piece++)
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        pieceScore += bitCount(whiteBB) * PIECE_SCORES[piece];
        pieceScore -= bitCount(blackBB) * PIECE_SCORES[piece];
    }
    
    // determine game phase based on total material
    bool lateGame = (abs(pieceScore) <= 2400);
    
    // evaluate non-pawn pieces normally
    for (Piece piece = KING; piece < PAWN; piece++)  // KING to KNIGHT (excludes PAWN)
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
    
    // evaluate pawns per file 
    Bitboard whitePawns = b.getUniquePiece(WHITE, PAWN);
    Bitboard blackPawns = b.getUniquePiece(BLACK, PAWN);
    
    for (size_t file = 0; file < 8; file++)
    {
        Bitboard fileMask = FileABB << file;
        Bitboard whitePawnsOnFile = whitePawns & fileMask;
        Bitboard blackPawnsOnFile = blackPawns & fileMask;
        
        if (whitePawnsOnFile)
        {
            positionScore += pawnEvalFile(b, file, WHITE, whitePawns, blackPawns);
        }
        if (blackPawnsOnFile)
        {
            positionScore -= pawnEvalFile(b, file, BLACK, blackPawns, whitePawns);
        }
    }
    
    return pieceScore + positionScore;
}