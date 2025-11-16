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

int main()
{
    MoveList list;
    Board board(perft_2);
    Bitboard blockers = generateBlockers(board, board.curSide);

    testMoveAmount(board);
    auto end = generateLegals(list.list.begin(), board, board.curSide);
    for(auto i = list.list.begin(); i < end; i++)
    {
        if(!isLegal(i, board, board.curSide, blockers)) { continue; }

        doMove(board, i);
        printPieceBoard(board);
        std::cout << (int)board.curState.hash;
        std::cin.get();
        undoMove(board, i);
        printPieceBoard(board);
        std::cout << (int)board.curState.hash;

        std::cin.get();
    }

    return 0;
}








void testMoveAmount(Board& board)
{

    MoveList list;
    auto end = list.list.begin();

    auto startTime = std::chrono::high_resolution_clock::now();
    end = generateLegals(end, board, board.curSide);
    auto endTime = std::chrono::high_resolution_clock::now();

    int legalCount = 0;
    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, board, board.curSide, generateBlockers(board, board.curSide)))
        {
            continue;
        }

        std::cout << "From: " << (int)m->getFrom() 
                << " To: " << (int)m->getTo()
                << " Piece: " << (int) m->getPiece()
                << " Capture: " << (int)m->isCapture()
                << " EP: " << (int)m->isEnpassant()
                << " Promo: " << (int)m->getPromoted()
                << " Castle: " << (int)m->isCastle()
                << "\n";
                legalCount++;

    }

    std::cout << "\nCOUNT OF MOVES: " << legalCount;

}