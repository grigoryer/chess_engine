#include "constants.hpp"
#include "debug.hpp"
#include "make_move.hpp"
#include "search.hpp"
#include <cstdarg>
#include <iostream>
#include <moves.hpp>
#include <sstream>
#include <thread>
#include <uci.hpp>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>

inline ExtdMove stringToMove(const std::string& moveStr, Board& b, Side s)
{
    ExtdMove move{};
    move.setMove(noSquare, noSquare, NONE);
    
    if (moveStr.length() < 4) { return move; }
    
    Square from = stringToSquare(moveStr.substr(0, 2));
    Square to = stringToSquare(moveStr.substr(2, 2));
    if (from == noSquare || to == noSquare) { return move; }
    
    Piece piece = b.pieceList[from];
    if (piece == NONE) { return move; }
    
    Piece captured = b.pieceList[to];
    Piece promotion = NONE;
    
    if (moveStr.length() >= 5) 
    {
        char promChar = moveStr[4];
        switch (promChar) 
        {
            case 'q': promotion = QUEEN; break;
            case 'r': promotion = ROOK; break;
            case 'b': promotion = BISHOP; break;
            case 'n': promotion = KNIGHT; break;
            default: break;
        }
    }
    
    bool isDouble = false;
    bool isEnpassant = false;
    bool isCastle = false;
    
    if (piece == PAWN) 
    {
        int rankDiff = abs(squareRank(to) - squareRank(from));
        if (rankDiff == 2) 
        {
            isDouble = true;
        }
        if (squareFile(from) != squareFile(to) && captured == NONE) 
        {
            isEnpassant = true;
        }
    } 
    else if (piece == KING) 
    {
        int fileDiff = abs(squareFile(to) - squareFile(from));
        if (fileDiff == 2) 
        {
            isCastle = true;
        }
    }
    
    move.setMove(from, to, piece, captured, promotion, isDouble, isEnpassant, isCastle);
    return move;
}

void UciEngine::setPosition(const std::string& input)
{
    std::istringstream iss(input);
    std::string tok;
    
    // Read first token to determine if startpos or fen
    iss >> tok;
    if (tok.empty()) { return; }
    
    if (tok == "startpos")
    {
        b.resetBoard(STARTING_FEN);
        
        // Look for moves keyword and process moves in one pass
        while (iss >> tok)
        {
            if (tok == "moves") { break; }
        }
        
        // Process remaining moves
        while (iss >> tok)
        {
            if (tok.length() >= 4)
            {
                ExtdMove move = stringToMove(tok, b, b.curSide);
                if (move.getFrom() != noSquare) { doMove(b, &move); }
            }
        }
    }
    else if (tok == "fen")
    {
        std::string fen;
        fen.reserve(100);
        
        // Read FEN parts until "moves" is encountered
        while (iss >> tok)
        {
            if (tok == "moves") { break; }
            if (!fen.empty()) { fen += " "; }
            fen += tok;
        }
        
        // Reset board with FEN
        if (!fen.empty()) { b.resetBoard(fen); }
        
        // Process remaining moves
        while (iss >> tok)
        {
            if (tok.length() >= 4)
            {
                ExtdMove move = stringToMove(tok, b, b.curSide);
                if (move.getFrom() != noSquare) { doMove(b, &move); }
            }
        }
    }
}

void UciEngine::searchWorker()
{
    while (!quitRequested.load())
    {
        ExtdMove bestMoveFound{};
        
        // Wait for search signal
        {
            std::unique_lock<std::mutex> lock(searchMutex);
            searchCV.wait(lock, [&] {
                return searchActive.load() || quitRequested.load();
            });
            if (quitRequested.load()) { return; }
        }
        
        // Extract search parameters
        s.stopFlag.store(false);
        auto startTime = std::chrono::steady_clock::now();
        
        std::thread searchingThread([this, &bestMoveFound]()
        {
            bestMoveFound = s.iterativeDeep(b, maxDepth);
            s.stopFlag.store(true); 
        });
        
        // Monitor time and search completion
        while (!s.stopFlag.load()) 
        {
            auto elapsed = std::chrono::steady_clock::now() - startTime;
            long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            if (elapsedMs >= timeForMove) 
            {
                s.stopFlag.store(true);   
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        searchingThread.join();
        
        // Output best move
        if (bestMoveFound.getFrom() != noSquare && bestMoveFound.getTo() != noSquare)
        {
            std::cout << "bestmove "
                    << SQUARE_NAMES[bestMoveFound.getFrom()]
                    << SQUARE_NAMES[bestMoveFound.getTo()];
            if (bestMoveFound.getPromoted() != NONE)
            {
                char p = 'q';
                if (bestMoveFound.getPromoted() == ROOK)   { p = 'r'; }
                if (bestMoveFound.getPromoted() == BISHOP) { p = 'b'; }
                if (bestMoveFound.getPromoted() == KNIGHT) { p = 'n'; }
                std::cout << p;
            }
            std::cout << "\n";
            std::cout.flush();
        }
        else
        {
            std::cout << "info NULL MOVE draw or mate" << std::endl;
            std::cout << "bestmove 0000\n";
            std::cout.flush();
        }
        
        searchActive.store(false);
    }
}

void UciEngine::commandWorker()
{
    while (!quitRequested.load())
    {
        std::string line;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [&] {
                return !commandQueue.empty() || quitRequested.load();
            });
            if (quitRequested.load()) { return; }
            line = std::move(commandQueue.front());
            commandQueue.pop();
        }
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "uci")
        {
            std::cout << "id name TeaTree\n";
            std::cout << "id author Grigory\n";
            std::cout << "uciok\n";
            std::cout.flush();
        }
        else if (cmd == "isready")
        {
            std::cout << "readyok\n";
            std::cout.flush();
        }
        else if (cmd == "ucinewgame")
        {
            s.tranposTable->resetTable();
            b.resetBoard(STARTING_FEN);
        }
        else if (cmd == "position")
        {
            std::string pos = line.substr(line.find("position") + 8);
            setPosition(pos);
        }
        else if (cmd == "go")
        {
            int maxDepth = 50;
            int wtime = 10000;
            int btime = 10000;
            int winc = 0;
            int binc = 0;
            
            std::istringstream goIss(line);
            std::string tok;
            while (goIss >> tok) 
            {
                if (tok == "depth")     { goIss >> maxDepth; }
                else if (tok == "wtime") { goIss >> wtime; }
                else if (tok == "btime") { goIss >> btime; }
                else if (tok == "winc")  { goIss >> winc; }
                else if (tok == "binc")  { goIss >> binc; }
            }
            
            int myTime = (b.curSide == WHITE) ? wtime : btime;
            int myInc  = (b.curSide == WHITE) ? winc  : binc;
            timeForMove = (myTime / 20) + (myInc / 2);
            
            searchActive.store(true);
            searchCV.notify_one();
        }
        else if (cmd == "print")
        {
            printDebug(b);
        }
    }
}

void UciEngine::loop()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "quit")
        {
            quitRequested.store(true);
            searchCV.notify_one();
            queueCV.notify_all();
            break;
        }
        else if (cmd == "stop")
        {
            s.stopFlag.store(true);
            continue;
        }
        else
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            commandQueue.push(line);
            queueCV.notify_one();
        }
    }
}

void UciEngine::run()
{
    std::thread searchThread(&UciEngine::searchWorker, this);
    std::thread worker(&UciEngine::commandWorker, this);
    loop();
    worker.join();
    searchThread.join();
}

/*
position startpos moves e2e4 c7c5 g1f3 e7e6 b2b3 a7a6 c1b2 d7d6 d2d4 c5d4 f3d4 g8f6 f1d3 b8d7 e1g1 g7g6 d1e1 f8h6 e4e5 d6e5 d4f3 e5e4 d3e4 e8g8 b3b4 f6e4 e1e4 d8b6 e4e2 e6e5 f3e5 b6b4 e5d7 b4b2 e2e5 b2e5 d7e5 h6g7 f2f4 g8h8 b1d2 c8f5 a1c1 a8c8 h2h3 f5c2 d2b3 c2f5 c1c8 f5c8 g1h2 h8g8 h2g3 g8h8 e5g4 h8g8 g3f2 g8h8 f2g1 h8g8 g1h1 g8h8 h1g1 h8g8 g1h1 g7b2 h1g1 g8h8 g1h1 c8e6 h1g1 e6d7 g1h1 d7c8 h1g1 c8e6 g1h1 b2a3 h1g1 a3d6 g1h1 h8g7 h1g1 b7b6 g1h1 e6f5 h1g1 f5e6 g1h1 e6f5 b3a1 f8c8 a1b3 c8c2 b3c1 d6f4 f1f4 c2c1 h1h2 c1e1 h2g3 e1d1 g3h2 d1e1 h2g3 e1d1 g3h2 f5b1 g4f2 d1c1 f4a4
go wtime 937 btime 117210 winc 600 binc 600

*/



/*



main loop and queue.
search thread.


position startpos moves d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 d1c2 b7b6 a2a3 b4c3 c2c3 c8b7 b2b3 e8g8 c1b2 d7d6 e1d1 b7e4 d1c1 h7h6 d4d5 e6d5 c4d5 e4d5 b3b4 d8d7 c1c2 b8c6 c2d1 d7g4 h2h3 g4c4 c3e1 c6b8 e1d2 c4b3 d1e1 f6h7 b2d4 b8c6 d4e3 c6e7 e3f4 b3a4 f4e3 a4b3 e3f4 b3a4 f4e3 a4a6 e3f4 a6b7 f4g3 d5g2 d2e3 g2h1 e3e7 g8h8 b4b5 h8g8 h3h4 h7f6 g3d6 c7d6 e7d6 g8h7 d6d1 h7g6 d1b1
go wtime 3177 btime 9792 winc 600 binc 600








*/