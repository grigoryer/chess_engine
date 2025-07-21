#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bitboards.hpp" // Adjust include as appropriate
#include "moves.hpp"
#include "types.hpp"


int main()
{
    Board board;
    srand(time(0));
    board.print_piece_list();
    //board.game_state.print_game_state();
    std::string input;
    
    while (std::getline(std::cin, input)) {
        if (input == "exit") break;
        
        // Check for undo command
        if (input == "p") {
            if (board.history.count != 0) {
                std::cout << "Undoing last move...\n";
                board.unmake_move();
                board.print_piece_list();
                //board.game_state.print_game_state();
            } else {
                std::cout << "No moves to undo.\n";
            }
            continue; // Skip the rest of the loop and wait for next input
        }
        
        U8* active_color = &board.game_state.active_color;
        board.gen_moves(*active_color);
        if (board.move_list.count == 0) {
            std::cout << " No legal moves available.\n";
            break;
        }
        
        size_t i = rand() % board.move_list.count;
        Move chosen = board.move_list.moves[i];
        
        board.make_move(chosen);
        board.print_piece_list();
        //board.game_state.print_game_state();
    }
    
    // Final state display (removed the automatic undo at the end)
    return 0;
}