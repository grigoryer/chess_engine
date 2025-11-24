#include "constants.hpp"
#include <debug.hpp>
#include <moves.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <iostream>
#include <string>



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



int perft(Board& b, int depth)
{
    if(depth == 0)
    {
        return 1;
    }

    MoveList list;
    auto end = list.list.begin();
    end = generateLegals(end, b, b.curSide);

    Bitboard blockers = generateBlockers(b, b.curSide);

    int legalCount = 0;
    U64 nodes = 0;

    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, b, b.curSide, blockers))
        {
            continue;
        }

        doMove(b, m);
        nodes += perft(b, depth - 1);
        undoMove(b, m);
    }
    return nodes;
}

int perftDivide(Board& b, int depth)
{
    MoveList list;
    auto end = list.list.begin();
    end = generateLegals(end, b, b.curSide);

    Bitboard blockers = generateBlockers(b, b.curSide);

    int legalCount = 0;
    U64 nodes = 0;

    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, b, b.curSide, blockers))
        {
            continue;
        }

        doMove(b, m);
        U64 curNodes = perft(b, depth - 1);
        std::string promo = "";
        switch (m->getPromoted()) 
        {
            case(NONE) : promo = promo; break;
            case(KNIGHT) : promo = "n"; break;
            case(QUEEN) : promo = "q"; break;
            case(BISHOP) : promo = "b"; break;
            case(ROOK) : promo = "r"; break;
        }
        std::cout << SQUARE_NAMES[m->getFrom()] << SQUARE_NAMES[m->getTo()] << promo << ": " << curNodes << "\n";
        undoMove(b, m);
        nodes += curNodes;
    }
    std::cout << "\nNodes searched: " << nodes << "\n";
    return nodes;
}
int perftDebug(Board& b, int depth)
{
    if (depth == 0) {
        return 1;
    }
    
    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    Bitboard blockers = generateBlockers(b, b.curSide);
    
    U64 nodes = 0;
    
    for (auto m = list.list.begin(); m != end; ++m) {
        if (!isLegal(m, b, b.curSide, blockers)) {
            continue;
        }

        doMove(b, m);
        printDebug(b);
        std::cin.get();
        
        nodes += perftDebug(b, depth - 1);
        
        undoMove(b, m);
        printDebug(b);
        std::cin.get();
    }
    
    return nodes;
}

void perftDivideDebug(Board& b, int depth)
{
    MoveList list;
    auto end = generateLegals(list.list.begin(), b, b.curSide);
    Bitboard blockers = generateBlockers(b, b.curSide);
    
    for (auto m = list.list.begin(); m != end; ++m) {
        if (!isLegal(m, b, b.curSide, blockers)) {
            continue;
        }
        
        // CHANGE THIS CONDITION AS NEEDED
        if (!m->isEnpassant()) {
            continue;
        }

        printDebug(b);
        std::cin.get();
        
        doMove(b, m);
        printDebug(b);
        std::cin.get();
        perftDebug(b, depth - 1);
        printDebug(b);
        std::cin.get();
        undoMove(b, m);
    }
}