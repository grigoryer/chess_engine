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
    Board board(perft_2);
    Bitboard blockers = generateBlockers(board, board.curSide);    

    int count = 100000;
    std::chrono::nanoseconds totalTime(0);
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

    std::cout << "\nAVG TIME TAKEN: " << (totalTime.count()/count);

    
    return 0;
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