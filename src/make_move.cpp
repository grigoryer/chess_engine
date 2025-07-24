#include "types.hpp"
#include <bitboards.hpp>

// make move and supporting functions
void Board::make_move(Move move)
{
    // phase 1 gather intel.
    const U8 side = game_state.active_color;
    const U8 piece = move.get_piece();
    const U8 from = move.get_source();
    const U8 to = move.get_target();
    const bool is_quiet = move.is_quiet(); 
    const bool is_capture = move.is_capture(); 
    const bool is_double = move.is_double(); 
    const bool is_promotion = move.is_promotion(); 
    determine_captured_piece(to);
    
    //phase 2 save data to history
    game_state.move = move;
    history.push(game_state);


    //phase 3 update game state;
    update_move_counters(side, piece, from ,is_capture,  is_promotion,  is_double);

    //phase 4 execute move;
    if(is_quiet){ move_piece(side, piece, from, to); }
    else{ move_support(side, piece, from, to, move, is_capture, is_promotion, is_double); }


    update_check_info();
    swap_sides();
}


void Board::determine_captured_piece(U8 to)
{
    U8 encoded_piece = piece_list[to]; 
    U8 captured_piece = (encoded_piece <= P) ? (encoded_piece) : (encoded_piece - NUM_PIECES);
    game_state.captured_piece = captured_piece;
}


void Board::update_move_counters(U8 side, U8 piece, U8 from, bool is_capture, bool is_promotion, bool is_double)
{
    update_halfclock(is_capture, is_promotion, piece);

    if(!is_double) { clear_epsquare(); }

    if(side == BLACK) { game_state.fullmove_number++; }

    castling_permissions_support(side, from, piece);
}

void Board::move_support(U8 side, U8 piece, U8 from, U8 to, Move move, bool is_capture, bool is_promotion, bool is_double)
{
    if(move.is_castling()) { castling_support(side, from, to); return; }
    if(move.is_enpassant()) { ep_support(move, side, from, to); return; }

    if(is_double) {  calculate_ep_num(side, to); }

    if(is_promotion) { piece = promotion_support(move); }

    if(is_capture) { remove_piece(opponent(), game_state.captured_piece, to); }

    move_piece(side, piece, from, to);
}


void Board::castling_support(U8 side, U8 king_from, U8 king_to)
{
    move_piece(side, KING, king_from, king_to);

    // determine rook movement based on direction
    const bool is_kingside = (king_to > king_from);

    U8 rook_from = 0;
    U8 rook_to = 0;

    if (side == WHITE) 
    {
        rook_from = is_kingside ? h1 : a1;
        rook_to = is_kingside ? f1 : d1;
        update_castling_permissions(game_state.castling & ~white_castling);
    } 
    else 
    {
        rook_from = is_kingside ? h8 : a8;
        rook_to = is_kingside ? f8 : d8;
        update_castling_permissions(game_state.castling & ~black_castling);
    }

    move_piece(side, ROOK, rook_from, rook_to);
}

void Board::castling_permissions_support(U8 side, U8 from, U8 piece)
{
    if(piece == ROOK) 
    {
        U8 new_castling = game_state.castling;

        if(from == h1) { new_castling &= ~wk; }       // White kingside
        else if(from == a1) { new_castling &= ~wq; } // White queenside  
        else if(from == h8) { new_castling &= ~bk; } // Black kingside
        else if(from == a8) { new_castling &= ~bq; }// Black queenside
        
        update_castling_permissions(new_castling);
    }

    if (piece == KING) 
    {
        update_castling_permissions(game_state.castling & ~(side == WHITE ? white_castling : black_castling));
    }
}

void Board::update_halfclock(bool is_capture, bool is_promotion, U8 piece)
{
    if(is_capture || is_promotion || piece == PAWN) 
    { 
        game_state.half_move_clock = 0; 
    }
    else 
    {
        game_state.half_move_clock++; 
    }
} 

U8 Board::promotion_support(Move move)
{
    U8 promoted_piece = 0;

    switch(move.get_promoted())
    {
        case(n_promotion): promoted_piece =  KNIGHT; break;
        case(b_promotion): promoted_piece = BISHOP; break;
        case(r_promotion): promoted_piece =  ROOK; break;
        case(q_promotion): promoted_piece = QUEEN; break; //NOLINT
        default: promoted_piece = QUEEN; break;
    }

    return promoted_piece;
}

void Board::ep_support(Move move, U8 side, U8 from, U8 to)
{
    const U8 captured_square = to + (opponent() == WHITE ? n_shift : -s_shift);
    remove_piece(opponent(), PAWN, captured_square);
    move_piece(side, PAWN, from, to);
}

void Board::calculate_ep_num(U8 side, U8 to)
{
    U8 ep_square = (to % BOARD_LENGTH) + ((side == WHITE ? 0 : BOARD_LENGTH));
    set_epsquare(ep_square);
}

void Board::move_piece(U8 side, U8 piece, U8 from, U8 to)
{
    remove_piece(side, piece, from);
    put_piece(side, piece, to);
}

void Board::remove_piece(U8 side, U8 piece, U8 square)
{
    pop_bit(bb_pieces[side][piece],square);
    pop_bit(bb_side[side],square);
    piece_list[square] = NONE; 
    game_state.zobrist_key ^= zobrist_randoms.piece(side, piece, square);
}

void Board::put_piece(U8 side, U8 piece, U8 square)
{
    set_bit(bb_pieces[side][piece],square);
    set_bit(bb_side[side],square);

    U8 piece_type = piece + (side == WHITE ? 0 : NUM_PIECES);
    
    piece_list[square] = piece_type; 
    game_state.zobrist_key ^= zobrist_randoms.piece(side, piece, square);
}

void Board::swap_sides()
{
    game_state.zobrist_key ^= zobrist_randoms.side(game_state.active_color);
    game_state.active_color ^= 1;
    game_state.zobrist_key ^= zobrist_randoms.side(game_state.active_color);
}

void Board::update_castling_permissions(U8 permissions)
{
    game_state.zobrist_key ^= zobrist_randoms.castling(game_state.castling);
    game_state.castling = permissions;
    game_state.zobrist_key ^= zobrist_randoms.castling(game_state.castling);
}

void Board::set_epsquare(U8 ep_square)
{
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
    game_state.en_passant = ep_square;
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
}

void Board::clear_epsquare()
{
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
    game_state.en_passant = ep_none;
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
}



// unmake move
void Board::unmake_move()
{
    if(history.count == 0) { return; }
    // phase 1 get previous state info (but don't restore game_state yet)
    GameState prev_state = history.top();
    history.pop();
    
    Move prev_move = prev_state.move;

    const U8 side = prev_state.active_color;  // Use the side that made the move
    const U8 piece = prev_move.get_piece();
    const U8 from = prev_move.get_source();
    const U8 to = prev_move.get_target();

    // phase 2 unmake move
    if(prev_move.is_quiet()) { move_piece(side, piece, to, from); }
    else { unmake_support(prev_move, side, piece, from, to, prev_state); }

    // phase 3 return game state.
    game_state = prev_state;
    update_check_info();
}


void Board::unmake_support(Move move, U8 side, U8 piece, U8 from, U8 to, const GameState& prev_state)
{
    if(move.is_castling()) { restore_castling(move, side); return; }
    if(move.is_enpassant()) { restore_enpassant(move, side, from, to, prev_state); return; }


    if(move.is_promotion()) 
    { 
        U8 promoted_piece = promotion_support(move);
        remove_piece(side, promoted_piece, to);
        put_piece(side, PAWN, from); 
    } 
    else 
    {
        move_piece(side, piece, to, from);
    }

    if(move.is_capture()) { restore_capture(move,prev_state, to ); }
}


void Board::restore_enpassant(Move move, U8 side, U8 from, U8 to, const GameState& prev_state)
{
    U8 enemy = prev_state.active_color ^ 1;
    U8 ep_num = enpassant_to_square(prev_state.en_passant);
    const U8 captured_square = ep_num + (enemy == WHITE ? n_shift : -s_shift);
    
    move_piece(side, PAWN, to, from);
    put_piece(enemy, PAWN, captured_square);
}

void Board::restore_castling(Move move, U8 side)
{
    const U8 king_from = move.get_source();
    const U8 king_to = move.get_target();
    
    // Move king back
    move_piece(side, KING, king_to, king_from);
    
    // Determine rook movement to reverse
    const bool is_kingside = (king_to > king_from);
    U8 rook_from = 0;
    U8 rook_to = 0;
    
    if (side == WHITE) 
    {
        rook_from = is_kingside ? h1 : a1;
        rook_to = is_kingside ? f1 : d1;
    } 
    else 
    {
        rook_from = is_kingside ? h8 : a8;
        rook_to = is_kingside ? f8 : d8;
    }
    // Move rook back
    move_piece(side, ROOK, rook_to, rook_from);
}

void Board::restore_capture(Move move, const GameState& prev_state, U8 square)
{
    U8 captured_piece = (prev_state.captured_piece <= P) ? prev_state.captured_piece 
    : (prev_state.captured_piece - NUM_PIECES);
    
    put_piece(prev_state.active_color ^ 1, captured_piece, square);
}
