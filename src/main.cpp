#include <attacks.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <iostream>
#include <constants.hpp>
#include <ostream>
#include <position.hpp>
#include <between.hpp>
#include <move_generation.hpp>



int main()
{
    Attacks::initializeAttacks();
    Between::initializeBetween();

    MoveList list;
    
    Board board(killer_position);
    
    Bitboard blockers = generateBlockers(board, board.curSide);

    auto end = list.list.begin();
    end = generateLegals(end, board, board.curSide);

    int legalCount = 0;
    for (auto m = list.list.begin(); m != end; ++m)
    {
        std::cout << squareArray[(int)m->getFrom()]
                << "->" << squareArray[(int)m->getTo()]
                << " Piece: " << (int) m->getPiece()
                << " EP: " << (int) m->isEnpassant()
                << " Is LEGAL: " << (isLegal(m, board, board.curSide, blockers) ? "YES" : "NO")
                << "\n";

                if(isLegal(m, board, board.curSide, blockers)) legalCount++;
    }

    std::cout << "COUNT OF MOVES: " << legalCount;
    printPieceBoard(board);

    
    return 0;
}




/*
    int count = 1000;
    std::chrono::nanoseconds totalTime;

    for(int i = 0; i < count; i++)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto end = list.list.begin();
        end = generateLegals(end, board, board.curSide);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        std::cout << "\nCOUNT: " << i;
        std::cout << "\nTime:  " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
        std::cout << "\n";
    }



    /*
    auto end = list.list.begin();

    auto startTime = std::chrono::high_resolution_clock::now();
    end = generateLegals(end, board, board.curSide);

    board.setCheckSqs(WHITE);
    auto check = board.isCheck(WHITE);

    auto endTime = std::chrono::high_resolution_clock::now();

    for (auto m = list.list.begin(); m != end; ++m)
    {
        std::cout << "From: " << (int)m->getFrom() 
                << " To: " << (int)m->getTo()
                << " Piece: " << (int) m->getPiece()
                << " Capture: " << (int)m->isCapture()
                << " EP: " << (int)m->isEnpassant()
                << " Promo: " << (int)m->getPromoted()
                << "\n";
    }

    std::cout << "COUNT OF MOVES: " << end - list.list.begin();

    auto avgTime = totalTime/count;
    std::cout << "\nAVG time taken: " << avgTime.count();
*/