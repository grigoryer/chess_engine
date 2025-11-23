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


void testMoveAmount(Board& board);
void testMoveAmount1(Board& board);

using std::cout;

int main()
{
    cout << "TEST: 1\n\n";
    Board b(killer_position);
    testMoveAmount1(b);
}








void testMoveAmount1(Board& board)
{

    MoveList list;
    auto end = list.list.begin();

    auto startTime = std::chrono::high_resolution_clock::now();
    end = generateLegals(end, board, board.curSide);
    auto endTime = std::chrono::high_resolution_clock::now();

    Bitboard blockers = generateBlockers(board, board.curSide);

    int legalCount = 0;

    printDebug(board);
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


       
        doMove(board, m);
        printDebug(board);
        std::cin.get();
        undoMove(board, m);
        printDebug(board);
        std::cin.get();
    }

    std::cout << "\nCount no legal check: " << end - list.list.begin();
    std::cout << "\nCOUNT OF MOVES: " << legalCount;

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

        Board copy = board;
    }

    std::cout << "\nCount no legal check: " << end - list.list.begin();
    std::cout << "\nCOUNT OF MOVES: " << legalCount;

}

/*

 MoveList list;
    Board board(perft_2);
    Bitboard blockers = generateBlockers(board, board.curSide);

    testMoveAmount(board);
    auto end = generateLegals(list.list.begin(), board, board.curSide);

    auto startTime = std::chrono::high_resolution_clock::now();
    for(auto i = list.list.begin(); i < end; i++)
    {
        if(!isLegal(i, board, board.curSide, blockers)) { continue; }

        doMove(board, i);
        undoMove(board, i);
    }
    auto endTime = std::chrono::high_resolution_clock::now();

    auto timeTaken = endTime - startTime;

    std::cout << "\nTime taken: " << timeTaken.count();
    return 0;
    
    */