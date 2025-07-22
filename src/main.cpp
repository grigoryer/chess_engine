#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bitboards.hpp" 


int main()
{
    Board board;

    print_board(board.get_piece(WHITE, PAWN));
    return 0;
}