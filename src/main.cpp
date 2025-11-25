#include <hash.hpp>
#include <debug.hpp>
#include <moves.hpp>
#include <iostream>
#include <constants.hpp>
#include <position.hpp>
#include <between.hpp>
#include <move_generation.hpp>
#include <make_move.hpp>
#include <search.hpp>
#include <evaluation.hpp>

using std::cout;


void playGame();

int main()
{
    Board b("8/1ppp3p/1b4p1/7p/8/8/2k1K2q/8 w - - 0 1");
    perftDivide(b, 1);
    playGame();
}


void playGame()
{

    Evaluation e;
    Search engine;
    Board b(perft_5);

    std::cout << e.evaluateBoard(b);
    printPieceBoard(b);

    while(true)
    {
        auto m = engine.search(b, 8);
        if(m == nullptr) 
        {
            if(b.isCheck(b.curSide)) { cout << "\nMATE\n"; }
            else {cout << "\nMATE\n";}

            perftDivide(b, 1);
            printDebug(b);
            break;
        }
        std::cout << SQUARE_NAMES[m->getFrom()] << SQUARE_NAMES[m->getTo()] << "\n";
        doMove(b, m);
        std::cout << e.evaluateBoard(b);
        printDebug(b);
        std::cin.get();
    }
}

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
    Board b;   
    auto startTime = std::chrono::high_resolution_clock::now();
    U64 nodes = perft(b, 6);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = endTime - startTime;
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
    
    cout << "Nodes: " << nodes << "\n";
    cout << "Time: " << nanos << " ns (" << seconds << " s)\n";
}