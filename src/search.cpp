#include "search.hpp"
#include "moves.hpp"
#include "types.hpp"

const int negative_infinity = -9999999;
const int positive_infinity = 9999999;

int Search::search(int depth)
{
    if(depth == 0){
        return evaluate(board);
    }
    board.gen_moves();
    MoveList moves = board.move_list;

    if(moves.get_count() == 0)
    {
        if (board.in_check(board.game_state.active_color)) {
            return negative_infinity;
        }
        return 0;
    }

    int best_eval = negative_infinity;

    for(int i = 0; i < moves.get_count(); i++)
    {
        Move move = moves.moves[i];
        
        // Check if the move is legal before making it
        if (!board.legal(move)) {
            continue; // Skip illegal moves
        }
        

        board.make_move(move);
        int evaluation = -search(depth - 1);
        board.unmake_move();

        if(evaluation > best_eval){
            best_eval = evaluation;

            if(depth == 3) { 
                best_move_found = move;
            }
        }
    }
    return best_eval;
}