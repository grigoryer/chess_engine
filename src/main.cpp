#include "bitboards.hpp"
#include "magic.hpp"


int main ()
{
    Board b;

    print_board(b.attack_tables.mask_bishop_attacks(c1));
    for(int i = 0; i < 4096; i++){
    print_board(b.attack_tables.gen_occupancy(i,b.attack_tables.mask_rook_attacks(a1)));}
    b.print_piece_list();

    return 0;
}
