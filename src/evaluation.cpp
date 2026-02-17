#include "attacks.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include <evaluation.hpp>
#include <iostream>

//piece in order are QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE
//all arrays are from own sides perspective, firs row = rank 1 for white and rank 8 for black

const Score sqScore = 5;

constexpr Square flipSquare(Square sq)
{
    return sq ^ 56;
}

inline Score Evaluation::scoreLeaperPosition(Piece piece, Square sq, bool isLateGame)
{
    switch(piece)
    {
        case(KING) : 
        { 
            return isLateGame ? KING_LATE_EVAL_MAP[sq] : KING_EVAL_MAP[sq]; 
        }
        case(PAWN) : 
        { 
            return isLateGame ? PAWN_LATE_EVAL_MAP[sq] : PAWN_EVAL_MAP[sq]; 
        }
        case(KNIGHT) : 
        {
            return KNIGHT_EVAL_MAP[sq]; 
        }
        default : {  }
    }
    return 0;
}


inline Score Evaluation::scoreSliderPosition(Piece piece, Square sq, bool isLateGame, Board& b, Side Side)
{
    Bitboard curSide = b.getSide(Side) ^ FullBB;

    switch(piece)
    {
        case(BISHOP) : 
        { 
            return (sqScore * bitCount(Attacks::getBishopAttacks(sq, b.occupancy) & curSide));
        }
        case(ROOK) : 
        { 
            return (sqScore * bitCount(Attacks::getRookAttacks(sq, b.occupancy) & curSide));
        }
        case(QUEEN) : 
        { 
            return (sqScore * bitCount(Attacks::getQueenAttacks(sq, b.occupancy) & curSide));
        }
        default : {  }
    }
    return 0;
}

Score Evaluation::evaluateBoard(Board& b)
{
    Score pieceScore = 0;
    Score positionScore = 0;
    bool lateGame = b.curState.phaseScore <= LATEGAME_PHASE;

    //king.
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, KING);
        Bitboard blackBB = b.getUniquePiece(BLACK, KING);

        while (whiteBB)
        {
            Square ksq = popLsb(whiteBB);
            Score curScore = (lateGame ? KING_LATE_EVAL_MAP[ksq] : KING_EVAL_MAP[ksq]);
            positionScore += curScore;
        }
        while (blackBB)
        {
            Square ksq = flipSquare(popLsb(blackBB));
            Score curScore = (lateGame ? KING_LATE_EVAL_MAP[ksq] : KING_EVAL_MAP[ksq]);
            positionScore -= curScore;
        }
    }
    
    //slider pieces
    for (Piece piece = QUEEN; piece <= BISHOP; piece++)
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        pieceScore += bitCount(whiteBB) * PIECE_SCORES[piece];
        pieceScore -= bitCount(blackBB) * PIECE_SCORES[piece];

        while (whiteBB)
        {
            Score curScore = scoreSliderPosition(piece, popLsb(whiteBB), lateGame, b, WHITE);
            positionScore += curScore;
        }
        while (blackBB)
        {
            Score curScore = scoreSliderPosition(piece, popLsb(blackBB), lateGame, b, BLACK);
            positionScore -= curScore;
        }
    }

    //leaper pieces
    for (Piece piece = KNIGHT; piece < PAWN; piece++)
    {
        Bitboard whiteBB = b.getUniquePiece(WHITE, piece);
        Bitboard blackBB = b.getUniquePiece(BLACK, piece);
        
        pieceScore += bitCount(whiteBB) * PIECE_SCORES[piece];
        pieceScore -= bitCount(blackBB) * PIECE_SCORES[piece];

        while (whiteBB)
        {
            Score curScore = scoreLeaperPosition(piece, popLsb(whiteBB), lateGame);
            positionScore += curScore;
        }
        while (blackBB)
        {
            Score curScore = scoreLeaperPosition(piece, flipSquare(popLsb(blackBB)), lateGame);
            positionScore -= curScore;
        }
    }


    //PAWN
    {
        constexpr Score PASSED_PAWN_BONUS    = 20;
        constexpr Score CONNECTED_PAWN_BONUS = 5;
        constexpr Score DOUBLED_PAWN_PENALTY = 15;

        Bitboard whiteBB = b.getUniquePiece(WHITE, PAWN);
        Bitboard blackBB = b.getUniquePiece(BLACK, PAWN);

        pieceScore += bitCount(whiteBB) * PIECE_SCORES[PAWN];
        pieceScore -= bitCount(blackBB) * PIECE_SCORES[PAWN];

        auto evalPawns = [&](Bitboard ownBB, Bitboard enemyBB, Side side) -> Score 
        {
            Score score = 0;
            Bitboard remaining = ownBB;

            while (remaining) 
            {
                Square sq  = popLsb(remaining);
                int file   = squareFile(sq);
                int rank   = squareRank(sq);

                // Position score
                Square scoreSq = (side == WHITE) ? sq : flipSquare(sq);
                score += scoreLeaperPosition(PAWN, scoreSq, lateGame);

                // Build masks
                Bitboard ownFile  = FileABB << file;
                Bitboard leftFile = ((file > 0) ? FileABB << (file - 1) : 0ULL);
                Bitboard rightFile = ((file < 7) ? FileABB << (file + 1) : 0ULL);

                // Passed pawn: no enemy on same or adjacent files ahead
                if (!(enemyBB & (ownFile | leftFile | rightFile)))
                {
                    score += PASSED_PAWN_BONUS;
                }

                // Connected pawns: ally on an adjacent file
                if (leftFile)
                {
                    score += CONNECTED_PAWN_BONUS;
                }
                if (rightFile)
                {
                    score += CONNECTED_PAWN_BONUS;
                }

                // Doubled pawns: more than one own pawn on this file
                if (bitCount(ownBB & ownFile) > 1)
                {
                    score -= DOUBLED_PAWN_PENALTY;
                }
            }
            return score;
        };

        positionScore += evalPawns(whiteBB, blackBB, WHITE);
        positionScore -= evalPawns(blackBB, whiteBB, BLACK);
    }

    return (pieceScore + positionScore);
}