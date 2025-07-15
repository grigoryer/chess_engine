#include "bitboards.hpp"
#include <sstream>
#include <vector>

void print_piece_list(Board b){
    std::cout << "\nPiece List Array:\n";
    for (int rank = 0; rank < 8; rank++) 
    {
        for (int file = 0; file < 8; file++) 
        {
            int square = rank * 8 + file;
            std::cout << b.piece_list[square] << "\t";
        }
        std::cout << "\n\n";
    }
}

int main ()
{

    Board b;

    std::cout << "=== GAME STATE DEBUG ===" << std::endl;
    std::cout << "Active Color: " << b.game_state.active_color << std::endl;
    std::cout << "Castling: " << b.game_state.castling << std::endl;
    std::cout << "En Passant: " << b.game_state.en_passant << std::endl;
    std::cout << "Half Move Clock: " << b.game_state.half_move_clock << std::endl;
    std::cout << "Fullmove Number: " << b.game_state.fullmove_number << std::endl;
    std::cout << "Zobrist Key: " << b.game_state.zobrist_key << std::endl;
    std::cout << "=========================" << std::endl;

    std::cout << b.occupancy();
    
    return 0;
}
