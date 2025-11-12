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
    Board board("rnbqkbnr/ppPp1pPp/8/4pP2/8/2NQ1BN1/P1PB2PP/R3K2R w KQkq e6 0 1");

    
    std::cout << "\n\nCURRENT EP: " << (int)board.curState.epSq;
    auto end = generateMoves<QUIET>(list.list.begin(), board);

    end = generateMoves<CAPTURE>(end, board);
    
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
    return 0;
}