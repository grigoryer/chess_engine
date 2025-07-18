#include "bitboards.hpp"
#include "types.hpp"
#include <time.h>

int main ()
{
    Board b;
    b.print_piece_list();

    srand(time(0));
    
    std::string input;


    while(true){
        std::getline(std::cin, input);
        if (input == "exit")
        {
            break;
        }

        b.gen_moves(b.game_state.active_color);
        
        int random;
        do {
            random = rand() % 256;
        } while(b.move_list.is_move_empty(random));
        
        b.make_move(b.move_list.moves[random]);
        b.print_piece_list();
    }


    std::cout << b.history.len();
    return 0;
}