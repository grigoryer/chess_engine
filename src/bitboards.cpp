#include "bitboards.hpp"
#include "zobrist.hpp"


ZobristRandoms zobrist_randoms;

void print_board(U64 bitboard) 
{
    std::cout << "\n";
    const U64 LAST_BIT = 63;

    for (int rank = 0; rank < 8; ++rank) {
        std::cout << 8 - rank << "  ";
        for (int file = 7; file >= 0; --file) {
            U64 mask = 1ULL << (LAST_BIT - (rank * 8 + file));
            char c = (bitboard & mask) ? '1' : '.';
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "   a b c d e f g h\n\n";
    std::cout << "#: " << bitboard << '\n';
}



U64 GameState::init_zobrist_key() const {
    U64 key = 0;

    for (int side = 0; side < NUM_SIDES; side++) 
    {
        for (int piece = 0; piece < NUM_PIECES; piece++) 
        {
            U64 bitboard = board.bb_pieces[side][piece];
            
            while (bitboard) 
            {
                int square = lsb(bitboard);
                key ^= zobrist_randoms.rnd_pieces[side][piece][square];
                pop_bit(bitboard,square);
            }
        }
    }

    for(int castling = 0; castling < NUM_CASTLING; castling++) { key ^= zobrist_randoms.rnd_castling[castling]; }
    for(int sides = 0; sides < NUM_SIDES; sides++) { key ^= zobrist_randoms.rnd_sides[sides]; }
    for(int en_passant = 0; en_passant < NUM_EPSQUARES; en_passant++) { key ^= zobrist_randoms.rnd_ep[en_passant]; }

    return key;    
}