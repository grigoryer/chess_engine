#include "constants.hpp"
#include <debug.hpp>
#include <moves.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <iostream>



void printBoard(Bitboard bitboard)
{
    using std::cout;

    cout << "\n";
    for (int rank = 7; rank >= 0; --rank)
    {
        cout << rank + 1 << " "; 
        for (int file = 0; file < NUM_FILES; ++file)
        {
            Square square = rank * 8 + file;
            char c = getBit(bitboard, square) ? '1' : '.';
            cout << c << ' ';
        }
        cout << '\n';
    }
    cout << "  a b c d e f g h\n\n";
    cout << "#: " << bitboard << '\n';
}

std::array<char, (NUM_PIECES * 2) + 1> uniquePieceArray
{
    'K', 'Q', 'R', 'B', 'N', 'P', '.', 'k', 'q', 'r', 'b', 'n', 'p',
};

void printPieceBoard(Board &b)
{
    using std::cout;
    cout << "\n";
    for (int rank = 7; rank >= 0; --rank)
    {
        cout << rank + 1 << " "; 
        for (int file = 0; file < NUM_FILES; ++file)
        {
            Square sq = rank * 8 + file;
            Piece piece = b.pieceList[sq];
            
            // Skip if no piece
            if (piece == NONE)
            {
                cout << ". ";
                continue;
            }
            
            bool isWhite = b.getSide(WHITE) & (1ULL << sq);
            int pieceArr = piece + (isWhite ? 0 : 7);
            char c = uniquePieceArray[pieceArr];
            cout << c << ' ';
        }
        cout << '\n';
    }
    cout << "  a b c d e f g h\n\n";
}


void printState(State& state)
{
    std::cout << "\n========== STATE DEBUG ==========\n";
    std::cout << "Hash: 0x" << std::hex << state.hash << std::dec << "\n";
    std::cout << "En Passant Square: " << static_cast<int>(state.epSq) << "\n";
    std::cout << "Castling Rights: " << static_cast<int>(state.castlingRights) << "\n";
    std::cout << "Halfmove Count: " << state.halfmoveCount << "\n";
    std::cout << "Fullmove Count: " << state.fullmoveCount << "\n";
    std::cout << "Captured Piece: " << static_cast<int>(state.capturedPiece) << "\n";
    std::cout << "================================\n\n";
}


void printDebug(Board& b)
{
    printPieceBoard(b);
    printState(b.curState);
}