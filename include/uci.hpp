#pragma once

#include <position.hpp>
#include "search.hpp"
#include "constants.hpp"



/*
need to include 
uci → respond with id name, id author, uciok
isready → respond with readyok
ucinewgame → clear internal caches if you have any
position → set board
go → start thinking
stop → stop thinking
bestmove → final move
quit → exit engine
*/

class UciEngine
{
    Board b;
    Search s;

    std::queue<std::string> commandQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::atomic<bool> quitRequested{false};
    bool hasActiveSearch = false;
    
    void setPosition(const std::string& input);
    void loop();
    void searchCommand(const std::string& inputLine);
    void commandWorker();

public: 

    void run();
};