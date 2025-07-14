#include "bitboards.hpp"
#include "zobrist.hpp"


int main ()
{
    GameState game;


    U64 white_rooks = game.board.bb_pieces[WHITE][ROOK];
    U64 black_rooks = game.board.bb_pieces[BLACK][ROOK];


    set_bit(white_rooks,a1);
    set_bit(white_rooks,h1);
    set_bit(black_rooks,h8);
    set_bit(black_rooks,a8);

    while(black_rooks)
    {
        std::cout << lsb(black_rooks) << " ";
        pop_bit(black_rooks,lsb(black_rooks));
    }

    return 0;
}
