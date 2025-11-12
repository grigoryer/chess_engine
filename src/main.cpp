#include <attacks.hpp>
#include <moves.hpp>
#include <iostream>
#include <constants.hpp>
#include <position.hpp>
#include <move_generation.hpp>



int main()
{


    Attacks::initializeAttacks();
    MoveList list;
    Board board(perft_5);

    
    auto end = generateMoves<QUIET>(list.list.begin(), board);

    end = generateMoves<CAPTURE>(end, board);
    for (auto m = list.list.begin(); m != end; ++m)
    {
        std::cout << "From: " << (int)m->getFrom() 
                << " To: " << (int)m->getTo()
                << " Piece: " << (int) m->getPiece()
                << " Capture: " << (int)m->isCapture()
                << "\n";
    }


    std::cout << "COUNT OF MOVES: " << end - list.list.begin();
    return 0;
}