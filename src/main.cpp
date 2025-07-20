#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include "bitboards.hpp" // Adjust include as appropriate

int main()
{
    Board b;
    srand(time(0));

    std::string input;

    while (true) {
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }

        b.gen_moves(b.game_state.active_color);

        std::vector<U16> castling_indices;
        for (U16 i = 0; i < 256; ++i) {
            if (!b.move_list.is_move_empty(i) && b.move_list.moves[i].is_castling()) {
                castling_indices.push_back(i);
            }
        }

        Move selected_move;

        if (!castling_indices.empty()) {
            int random_index = rand() % castling_indices.size();
            selected_move = b.move_list.moves[castling_indices[random_index]];
        } else {
            // Fallback: choose any valid move
            std::vector<U16> legal_indices;
            for (U16 i = 0; i < 256; ++i) {
                if (!b.move_list.is_move_empty(i)) {
                    legal_indices.push_back(i);
                }
            }

            if (!legal_indices.empty()) {
                int random_index = rand() % legal_indices.size();
                selected_move = b.move_list.moves[legal_indices[random_index]];
            } else {
                std::cout << "No legal moves available. Game over.\n";
                break;
            }
        }

        b.make_move(selected_move);
        b.print_piece_list();
        std::cout << (int)bits_in_bitboard(b.occupancy());
        b.game_state.print_game_state();
    }

    std::cout << b.history.len();
    return 0;
}
