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
#include <vector>
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
        switch (promChar) {
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
    if (tok.empty()) return;
    
    if (tok == "startpos")
    {
        b.resetBoard(STARTING_FEN);
        
        //look for moves keyword and process moves in one pass
        while (iss >> tok)
        {
            if (tok == "moves") break;
        }
        
        //process remaining moves
        while (iss >> tok)
        {
            if (tok.length() >= 4) //make sure its valid
            {
                ExtdMove move = stringToMove(tok, b, b.curSide);
                if (move.getFrom() != noSquare)  { doMove(b, &move); }
            }
        }
    }
    else if (tok == "fen")
    {
        std::string fen;
        fen.reserve(100); // Pre-allocate space
        
        //read FEN parts until "moves" is encountered
        while (iss >> tok)
        {
            if (tok == "moves") break;
            if (!fen.empty()) fen += " ";
            fen += tok;
        }
        
        //reset board with FEN
        if (!fen.empty()) { b.resetBoard(fen); }
        
        //process remaining moves
        while (iss >> tok)
        {
            if (tok.length() >= 4)
            {
                ExtdMove move = stringToMove(tok, b, b.curSide);
                if (move.getFrom() != noSquare) doMove(b, &move);
            }
        }
    }
}

void UciEngine::searchCommand(const std::string& inputLine)
{
    int depth = 7;
    int wtime = 60, btime = 60;
    int winc = 0, binc = 0;
    std::istringstream goIss(inputLine);
    std::string tok;
    //std::cout << "\ninput line: " << inputLine;
    
    while (goIss >> tok) 
    {
        if (tok == "depth")     goIss >> depth;
        else if (tok == "wtime") goIss >> wtime;
        else if (tok == "btime") goIss >> btime;
        else if (tok == "winc")  goIss >> winc;
        else if (tok == "binc")  goIss >> binc;
    }
    
    int myTime = (b.curSide == WHITE) ? wtime : btime;
    int myInc  = (b.curSide == WHITE) ? winc  : binc;
    int timeForMove = (myTime / 15) + myInc; //arbiturary time limit
    
    
    s.stopFlag.store(false);
    ExtdMove bestMoveFound{};
    auto startTime = std::chrono::steady_clock::now();
    
    std::thread workerThread([this, &bestMoveFound, depth]()
    {
        bestMoveFound = s.search(b, depth);  
        s.stopFlag.store(true); 
    });
    
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
    
    workerThread.join();
    hasActiveSearch = false;
    
    //print move if legal if mate or stalemate print 0000

        std::cout << "info selected move "
                << SQUARE_NAMES[bestMoveFound.getFrom()]
                << SQUARE_NAMES[bestMoveFound.getTo()];

    if (bestMoveFound.getFrom() != noSquare && bestMoveFound.getTo() != noSquare)
    {
        std::cout << "bestmove "
                << SQUARE_NAMES[bestMoveFound.getFrom()]
                << SQUARE_NAMES[bestMoveFound.getTo()];

        if (bestMoveFound.getPromoted() != NONE)
        {
            char p = 'q';
            if (bestMoveFound.getPromoted() == ROOK)   p = 'r';
            if (bestMoveFound.getPromoted() == BISHOP) p = 'b';
            if (bestMoveFound.getPromoted() == KNIGHT) p = 'n';
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
}

void UciEngine::commandWorker()
{
    int posCount = 1;
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
            std::cout << "id name Medici\n";
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
            b.resetBoard(STARTING_FEN);
        }
        else if (cmd == "position")
        {
            //std::cout << "info position start: " << posCount << std::endl;
            std::string pos = line.substr(line.find("position") + 8);
            setPosition(pos);
            //std::cout << "info position end: " <<  posCount++ << std::endl;
        }
        else if (cmd == "go")
        {
            std::thread(&UciEngine::searchCommand, this, line).detach();
            hasActiveSearch = true;
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

        //std::cout << "info command given: " << line << std::endl;
        if (cmd == "quit")
        {
            quitRequested.store(true);
            s.stopFlag.store(true);
            queueCV.notify_all();
            break;
        }
        else if (cmd == "stop")
        {
            s.stopFlag.store(true);
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            commandQueue.push(line);
            //std::cout << "info added command to queue: " << line << std::endl;

        }
        queueCV.notify_one();
    }
}

void UciEngine::run()
{
    std::thread worker(&UciEngine::commandWorker, this);
    loop();
    worker.join();
}

/*
position startpos moves d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8a6 d1a4 a6b7 f1g2 c7c5 d4c5 f8c5 e1g1 c5e7 b1c3 g7g6 c1h6
isready
go wtime 9208 btime 9988
*/