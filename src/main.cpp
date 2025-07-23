#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bitboards.hpp" 

int main()
{
    Board board;

    std::cout << board.perft(6);
    board.print_piece_list();
}
