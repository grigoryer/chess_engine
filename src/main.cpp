#include "uci.hpp"
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
void initNameSpaces();



int main()
{
    UciEngine game;
    initNameSpaces();

    game.run();
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


void initNameSpaces()
{
    Attacks::initializeAttacks();
    Between::initializeBetween();
    ZobristHashing::initializeZobristRandoms();
}