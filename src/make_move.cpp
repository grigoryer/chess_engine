#include "types.hpp"
#include <bitboards.hpp>
#include <iostream>

// make move and supporting functions
void Board::make_move(Move move)
{
    game_state.move = move;
    history.push(game_state);

    const U8 side = us();
    const U8 piece = move.get_piece();
    const U8 from = move.get_source();
    const U8 to = move.get_target();

    const bool is_capture = move.is_capture();
    const bool is_promotion = move.is_promotion();
    const bool is_double = move.is_double();

    update_halfclock(is_capture, is_promotion, piece);

    if(!is_double) { clear_epsquare(); }

    if(is_capture && !is_promotion) 
    { 
        capture_support(to); 
        move_piece(side, piece, from, to);
        swap_sides();
    }

    if(side == BLACK) { game_state.fullmove_number++; }
    if(piece == ROOK || piece == KING) { castling_permissions_support(side, from, piece); }

    if(move.is_quiet()) { move_piece(side, piece, from, to); swap_sides(); return; }
    if(move.is_double()) { double_support(side, from, to); return;}
    if(move.is_castling()) { castling_support(side, from, to); return; }
    if(move.is_enpassant()) { ep_support(move, side, from, to); return; }
    if(move.is_promotion()) { promotion_support(move, side, from, to); return; }
}

void Board::castling_support(U8 side, U8 king_from, U8 king_to)
{
    move_piece(side, KING, king_from, king_to);

    // Determine rook movement based on direction
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

    clear_epsquare();
    move_piece(side, ROOK, rook_from, rook_to);
    swap_sides();
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
    if(is_capture|| is_promotion || piece == PAWN) 
    { 
        game_state.half_move_clock = 0; 
    }
    else 
    {
        game_state.half_move_clock++; 
    }
} 

void Board::promotion_support(const Move move, U8 side, U8 from, U8 to)
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

    remove_piece(side, PAWN, from);
    put_piece(side, promoted_piece, to);
    swap_sides();
}

void Board::capture_support(U8 to)
{
    U8 encoded_piece = piece_list[to];
    U8 captured_piece = (encoded_piece <= P) ? (encoded_piece) : (encoded_piece - NUM_PIECES);
    game_state.captured_piece = encoded_piece;
    remove_piece(opponent(), captured_piece, to);
}

void Board::ep_support(Move move, U8 side, U8 from, U8 to)
{
    U8 ep = enpassant_to_square(game_state.en_passant);
    const U8 captured_square = ep + (opponent() == WHITE ? n_shift : -s_shift);

    game_state.captured_piece = PAWN;
    remove_piece(opponent(), PAWN, captured_square);
    move_piece(side, PAWN, from, to);
    swap_sides();
}

void Board::double_support(U8 side, U8 from, U8 to)
{
    U8 ep_square = (to % BOARD_LENGTH) + ((side == WHITE ? 0 : BOARD_LENGTH));
    set_epsquare(ep_square);
    move_piece(side, PAWN, from, to);
    swap_sides();
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

    std::cout << piece_type;
    
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
    
    GameState prev_state = history.top();
    Move prev_move = prev_state.move;

    const U8 side_that_moved = opponent();
    const U8 piece = prev_move.get_piece();
    const U8 from = prev_move.get_source();
    const U8 to = prev_move.get_target();
    const bool is_promotion = prev_move.is_promotion();

    if(is_promotion) 
    {
        unmake_promotion(prev_move, side_that_moved, from, to, prev_state);
    }
    else if(prev_move.is_enpassant()) 
    {
        unmake_enpassant(side_that_moved, from, to, prev_state);
    }
    else if(prev_move.is_castling()) 
    {
        unmake_castling(prev_move);
    }
    else if(prev_move.is_capture() && !is_promotion)
    {
        move_piece(side_that_moved, piece, to, from);
        unmake_capture(prev_move, prev_state);
    }
    else 
    {
        move_piece(side_that_moved, piece, to, from);
    }

    swap_sides();
    game_state = prev_state;
    history.pop();
}

void Board::unmake_promotion(const Move& move, U8 side, U8 from, U8 to, const GameState& prev_state)
{
    U8 promoted_piece = 0;
    switch(move.get_promoted())
    {
        case(n_promotion): promoted_piece = KNIGHT; break;
        case(b_promotion): promoted_piece = BISHOP; break;
        case(r_promotion): promoted_piece = ROOK; break;
        case(q_promotion): promoted_piece = QUEEN; break; //NOLINT
        default: promoted_piece = QUEEN; break;
    }

    remove_piece(side, promoted_piece, to);
    put_piece(side, PAWN, from);
    
    if(move.is_capture())
    {
        U8 captured_piece = (prev_state.captured_piece <= P) 
        ? prev_state.captured_piece : (prev_state.captured_piece - NUM_PIECES);
        put_piece(opponent(), captured_piece, to);
    }
}


void Board::unmake_enpassant(U8 side, U8 from, U8 to, const GameState& prev_state)
{
    U8 enemy = opponent();

    U8 ep_num = enpassant_to_square(prev_state.en_passant);
    const U8 captured_square = ep_num + (enemy == WHITE ? n_shift : -s_shift);
    
    move_piece(side, PAWN, to, from);
    put_piece(enemy, PAWN, captured_square);
}


void Board::unmake_castling(const Move& move)
{
    const U8 side = us();
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

void Board::unmake_capture(const Move& move, const GameState& prev_state)
{
    U8 captured_piece = (prev_state.captured_piece <= P) ? 
                       prev_state.captured_piece : 
                       (prev_state.captured_piece - NUM_PIECES);
    
    put_piece(opponent(), captured_piece, move.get_target());
}

U8 piece_to_piecelist(U8 piece_list_type)
{
    return 1;
}

U8 piecelist_to_piece(U8 piece)
{

    return 1;
}