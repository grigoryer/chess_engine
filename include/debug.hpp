#pragma once
#include <constants.hpp>
#include <position.hpp>
#include <iostream>

//debug functions
inline void printBoard(Bitboard bitboard)
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

inline std::array<char, (NUM_PIECES * 2) + 1> uniquePieceArray
{
    'K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p', '.'
};

inline void printPieceBoard(Board &b)
{
    using std::cout;

    cout << "\n";
    for (int rank = 7; rank >= 0; --rank)
    {
        cout << rank + 1 << " "; 
        for (int file = 0; file < NUM_FILES; ++file)
        {
            Square sq = rank * 8 + file;
            char c = uniquePieceArray[static_cast<int>(b.pieceList[sq])];
            cout << c << ' ';
        }
        cout << '\n';
    }
    cout << "  a b c d e f g h\n\n";
}

