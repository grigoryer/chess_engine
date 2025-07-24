#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bitboards.hpp" 


int main()
{
    Board board;
    board.print_piece_list();

    std::cout << board.perft(2);
}
