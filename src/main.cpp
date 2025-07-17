#include "bitboards.hpp"
#include "types.hpp"

int main ()
{
    Board b;

    b.print_piece_list();
    b.gen_white_moves();
    
    return 0;
}
