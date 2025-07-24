#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bitboards.hpp" 
#include "search.hpp"


int main()
{
    Board board;
    Search engine(board);


    board.print_piece_list();

    while(std::cin.get()){

        engine.search(3);

        engine.best_move_found.print_move(board.game_state.active_color);
        board.make_move(engine.best_move_found);
        board.print_piece_list();
        board.game_state.print_game_state();
    }


}