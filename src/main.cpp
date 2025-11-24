#include <hash.hpp>
#include <attacks.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <iostream>
#include <constants.hpp>
#include <position.hpp>
#include <between.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>

using std::cout;
void profileCheck()
{
    Board b(perft_2);
    MoveList ml;
    printPieceBoard(b);
    auto end = generateLegals(ml.list.begin(), b, b.curSide);
    
    auto move = end - 5; 
    // Only profile move generation, not perft
    for (U64 i = 0; i < 100000000000; i++) 
    {
        updateChecking(b, move->getPiece(), move->getFrom(), move->getTo());
    }
}

void profilePerft()
{
    Board b(perft_2);   
    auto startTime = std::chrono::high_resolution_clock::now();
    U64 nodes = perft(b, 6);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = endTime - startTime;
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
    
    cout << "Nodes: " << nodes << "\n";
    cout << "Time: " << nanos << " ns (" << seconds << " s)\n";
}

void testMoveAmount(Board& board)
{

    MoveList list;
    auto end = list.list.begin();

    auto startTime = std::chrono::high_resolution_clock::now();
    end = generateLegals(end, board, board.curSide);
    auto endTime = std::chrono::high_resolution_clock::now();

    Bitboard blockers = generateBlockers(board, board.curSide);
    int legalCount = 0;
    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, board, board.curSide, blockers))
        {
            continue;
        }

        std::cout << "From: " << SQUARE_NAMES[(int)m->getFrom()] 
                << " To: " << SQUARE_NAMES[(int)m->getTo()]
                << " Piece: " << (int) m->getPiece()
                << " Capture: " << (int)m->getCapture()
                << " EP: " << (int)m->isEnpassant()
                << " Promo: " << (int)m->getPromoted()
                << " Castle: " << (int)m->isCastle()
                << "\n";
                legalCount++;
    }

    std::cout << "\nCount no legal check: " << end - list.list.begin();
    std::cout << "\nCOUNT OF MOVES: " << legalCount;

}



void perftDivide2(Board& b, int depth)
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
        U64 curNodes = perftDivide(b, depth - 1);
        std::string promo = "";
        switch (m->getPromoted()) 
        {
            case(NONE) : promo = promo; break;
            case(KNIGHT) : promo = "n"; break;
            case(QUEEN) : promo = "q"; break;
            case(BISHOP) : promo = "b"; break;
            case(ROOK) : promo = "r"; break;
        }
        std::cout << SQUARE_NAMES[m->getFrom()] << SQUARE_NAMES[m->getTo()] << promo << ": " << curNodes << "\n\n\n";
        undoMove(b, m);
        nodes += curNodes;
    }
    std::cout << "\nNodes searched: " << nodes << "\n";
}

int main()
{
    //Board b("8/2p5/K2p4/1P5r/7k/8/4P1p1/7R b - - 0 1");
    Board b;
    //printDebug(b);
    //perftDebug(b, 2);
    //perftDivideDebug(b, 2);
    perftDivide(b, 7);
    //printDebug(b);

    //testMoveAmount(b);
    


}



/*
inline Alignment getAlignment(Square s1, Square s2)
{
    if (s1 == s2) return Alignment::NONE;
    
    U8 rank1 = s1 >> 3, rank2 = s2 >> 3;
    U8 file1 = s1 & 0x7, file2 = s2 & 0x7;
    
    if (rank1 == rank2) return Alignment::STRAIGHT;
    if (file1 == file2) return Alignment::STRAIGHT;
    
    int rank_diff = rank1 - rank2;
    int file_diff = file1 - file2;
    if (rank_diff * rank_diff == file_diff * file_diff)  // |rank_diff| == |file_diff|
        return Alignment::DIAGONAL;
    
    return Alignment::NONE;
}
    */