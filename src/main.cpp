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

void testMoveAmount(Board& board);
void playGame();
void profilePerft();

int main()
{
    // Board b(perft_2);
    // MoveList list;
    // auto end = generateLegals(list.list.begin(), b, b.curSide);

    // int legalCount = scoreMoveList(b, list, end);
    // int count = 0;
    // for (auto m = list.list.begin(); m <  list.list.begin() + legalCount; ++m)
    // {
    //     std::cout << "Move: " << SQUARE_NAMES[(int)m->getFrom()]  << SQUARE_NAMES[(int)m->getTo()]
    //             << " Piece: " << (int) m->getPiece()
    //             << " Capture: " << (int)m->getCapture()
    //             << " EP: " << (int)m->isEnpassant()
    //             << " Promo: " << (int)m->getPromoted()
    //             << " Castle: " << (int)m->isCastle()
    //             << "\n";
    //             count++;
    // }


    // std::cout << "\nCOUNT OF MOVES: " << legalCount << "\n";
    // std::cout << "\nCOUNT OF MOVES: " << count << "\n\n\n";

    // testMoveAmount(b);

    playGame();
}

/*
Evaluation e;
    Search engine;
    Board b(perft_2);
    MoveList list;  
    auto end = generateQuiescence(list.list.begin(), b,  b.curSide);
    Bitboard blockers = generateBlockers(b, b.curSide);

    printDebug(b);
    for (auto m = list.list.begin(); m != end; ++m)
    {
         if(!isLegal(m, b, b.curSide, blockers))
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
    }
                */


void playGame()
{
    Evaluation e;
    Search engine;
    Board b(killer_position);

    std::cout << e.evaluateBoard(b);
    printPieceBoard(b);

    while(true)
    {
        auto m = engine.search(b, 7);
        if(m == nullptr) 
        {
            if(b.isCheck(b.curSide)) { cout << "\nMATE\n"; }
            else {cout << "\nSTALEMATE\n";}

            perftDivide(b, 1);
            printDebug(b);
            break;
        }
        std::cout << SQUARE_NAMES[m->getFrom()] << SQUARE_NAMES[m->getTo()] << "\n";
        doMove(b, m);

        std::cout << e.evaluateBoard(b);
        printDebug(b);
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
    U64 nodes = perft(b, 5);
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

    int legalCount = 0;
    for (auto m = list.list.begin(); m != end; ++m)
    {
        if(!isLegal(m, board, board.curSide, generateBlockers(board, board.curSide)))
        {
            continue;
        }

        std::cout << "Move: " << SQUARE_NAMES[(int)m->getFrom()]  << SQUARE_NAMES[(int)m->getTo()]
                << " Piece: " << (int) m->getPiece()
                << " Capture: " << (int)m->getCapture()
                << " EP: " << (int)m->isEnpassant()
                << " Promo: " << (int)m->getPromoted()
                << " Castle: " << (int)m->isCastle()
                << "\n";
                legalCount++;
    }

    std::cout << "\nCOUNT OF MOVES: " << legalCount;

}