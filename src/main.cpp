#include "bitboards.hpp"


int main ()
{
    
    Board b;

        
    set_bit(b.bb_pieces[WHITE][ROOK], a1);
    set_bit(b.bb_pieces[WHITE][ROOK], h1);
    set_bit(b.bb_pieces[WHITE][KNIGHT], b1);
    set_bit(b.bb_pieces[WHITE][KNIGHT], g1);
    set_bit(b.bb_pieces[WHITE][BISHOP], c1);
    set_bit(b.bb_pieces[WHITE][BISHOP], f1);
    set_bit(b.bb_pieces[WHITE][QUEEN], d1);
    set_bit(b.bb_pieces[WHITE][KING], e1);
    set_bit(b.bb_pieces[WHITE][PAWN], a2);
    set_bit(b.bb_pieces[WHITE][PAWN], b2);
    set_bit(b.bb_pieces[WHITE][PAWN], c2);
    set_bit(b.bb_pieces[WHITE][PAWN], d2);

    set_bit(b.bb_pieces[WHITE][PAWN], e2);
    set_bit(b.bb_pieces[WHITE][PAWN], f2);
    set_bit(b.bb_pieces[WHITE][PAWN], g2);
    set_bit(b.bb_pieces[WHITE][PAWN], h2);


    // Set some BLACK pieces
    set_bit(b.bb_pieces[BLACK][ROOK], a8);
    set_bit(b.bb_pieces[BLACK][ROOK], h8);
    set_bit(b.bb_pieces[BLACK][KNIGHT], b8);
    set_bit(b.bb_pieces[BLACK][KNIGHT], g8);
    set_bit(b.bb_pieces[BLACK][BISHOP], c8);
    set_bit(b.bb_pieces[BLACK][QUEEN], d8);
    set_bit(b.bb_pieces[BLACK][KING], e8);
    set_bit(b.bb_pieces[BLACK][PAWN], a7);
    set_bit(b.bb_pieces[BLACK][PAWN], h7);
    set_bit(b.bb_pieces[BLACK][PAWN], e5);

    print_board(b.occupancy());

    return 0;
}
