#include "bitboards.hpp"
#include "types.hpp"
#include <iostream>

void Board::make_move(const Move move)
{   
    game_state.move = move;
    history.push(game_state);

    U8 side = us();
    U8 piece = move.get_piece();
    U8 source = move.get_source();
    U8 target = move.get_target();
    U8 promoted = move.get_promoted();
    bool castling = move.is_castling();


    if(move.is_capture() == true) 
    {
        capture_support(target);
    }

    if(promoted != 0) 
    {
        U8 promoted_piece = promotion_support(promoted);
        remove_piece(side, piece, source);
        put_piece(side, promoted_piece, target);
    }
    else 
    {
        move_piece(side, piece, source, target);
    }

    move_support(side, piece, target, source, move);
    move.print_move(side);
}

void Board::move_support(U8 side, U8 piece, U8 target, U8 source, const Move& move)
{
    if(move.is_castling()) { castling_support(target); }
    if(move.is_enpassant()) { enpassant_support(); }

    if(move.is_double() == true) { double_support(target); }

    if(move.is_capture() || move.is_promotion() || piece == PAWN) 
    { 
        game_state.half_move_clock = 0; 
    }
    else 
    {
        game_state.half_move_clock++; 
    }

    if(!move.is_double()) { clear_epsquare(); }
    
    if(piece == ROOK || piece == KING) {castling_permissions_support(piece, source, side); }

    if(side == BLACK) { game_state.fullmove_number++;}

    move_list.clear();

    swap_sides();
}

void Board::capture_support(U8 square)
{
    U8 encoded_piece = piece_list[square];
    U8 enemy = opponent();
    
    // Convert encoded piece to piece type
    U8 piece_type = (encoded_piece <= P) ? (encoded_piece - 1) : (encoded_piece - 7);
    
    remove_piece(enemy, piece_type, square);
}

void Board::castling_permissions_support(U8 piece, U8 from, U8 side)
{
    if(piece == ROOK) 
    {
        U8 new_castling = game_state.castling;

        if(from == h1) new_castling &= ~wk;      // White kingside
        else if(from == a1) new_castling &= ~wq; // White queenside  
        else if(from == h8) new_castling &= ~bk; // Black kingside
        else if(from == a8) new_castling &= ~bq; // Black queenside
        
        update_castling_permissions(new_castling);
    }

    if (piece == KING) 
    {
        // Remove both castling rights for the moving side
        update_castling_permissions(game_state.castling & ~(side == WHITE ? white_castling : black_castling));
    }
}


U8 Board::promotion_support(U8 promotion)
{
    switch(promotion)
    {
        case(N): return KNIGHT;
        case(B): return BISHOP;
        case(R): return ROOK;
        case(Q): return QUEEN; //NOLINT
        default: return QUEEN; 
    }
}

void Board::castling_support(U8 king_target)
{
    if(king_target == g1) 
    { 
        remove_piece(WHITE, ROOK, h1);
        put_piece(WHITE, ROOK, f1);
        update_castling_permissions(game_state.castling & ~white_castling);
    }
    else if(king_target == c1) 
    { 
        remove_piece(WHITE, ROOK, a1);
        put_piece(WHITE, ROOK, d1);
        update_castling_permissions(game_state.castling & ~white_castling);
    }
    else if(king_target == g8) 
    { 
        remove_piece(BLACK, ROOK, h8);
        put_piece(BLACK, ROOK, f8);
        update_castling_permissions(game_state.castling & ~black_castling);
    }
    else if(king_target == c8) 
    { 
        remove_piece(BLACK, ROOK, a8);
        put_piece(BLACK, ROOK, d8);
        update_castling_permissions(game_state.castling & ~black_castling);
    }
}

void Board::enpassant_support()
{
    U8 const ep_split = 7;
    U8 const double_row_white = 4;
    U8 const double_row_black = 5;

    U8 ep = game_state.en_passant;

    //find pawn to delete due to ep.
    U8 target_square = ep + (ep < ep_split ? double_row_white : double_row_black);
    set_epsquare(ep_none);
    remove_piece(opponent(), PAWN, target_square);
}

void Board::double_support(U8 target)
{
    U8 ep_square = (target % BOARD_LENGTH) + (us() == WHITE ? 0 : BOARD_LENGTH);
    set_epsquare(ep_square);
    game_state.en_passant = ep_square;
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

   U8 piece_type = piece + 1 + (side == BLACK ? NUM_PIECES : 0);

    piece_list[square] = piece_type; 
    game_state.zobrist_key ^= zobrist_randoms.piece(side, piece, square);
}

void Board::swap_sides()
{
    game_state.zobrist_key ^= zobrist_randoms.side
    (game_state.active_color);
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

void Board::gen_moves(U8 side)
{
    move_list.clear();
    if(side == 0){
        gen_white_moves();
    }
    else
    {
        gen_black_moves();
    }
}

void Board::gen_white_moves()
{
    U8 side = WHITE;
    U64 occupied = occupancy();
    U64 enemies = bb_side[BLACK];

    gen_king_moves(side, occupied, enemies);
    gen_white_pawn_moves(occupied,enemies);
    gen_knight_moves(side, enemies);
    gen_bishop_moves(side, occupied, enemies);
    gen_rook_moves(side, occupied, enemies);
    gen_queen_moves(side, occupied, enemies);

}

void Board::gen_black_moves()
{
    U8 side = BLACK;
    U64 occupied = occupancy();
    U64 enemies = bb_side[WHITE];

    gen_king_moves(side, occupied, enemies);
    gen_black_pawn_moves(occupied,enemies);
    gen_knight_moves(side, enemies);
    gen_bishop_moves(side, occupied, enemies);
    gen_rook_moves(side, occupied, enemies);
    gen_queen_moves(side, occupied, enemies);

}

void Board::gen_white_pawn_moves(U64 occupancy, U64 enemies)
{
    U64 pawns = bb_pieces[WHITE][PAWN];

    while(pawns != 0)
    {
        U8 from = lsb(pawns);
        U8 to = from + n_shift;

        if(to <= h8 && !get_bit(occupancy, to))
        {
            if(from >= a7) // Promotion
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, false, false, false, false));

            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, false, false, false, false));
                
                if(from >= a2 && from <= h2 && !get_bit(occupancy, to + n_shift))
                  //std::cout << "pawn " << from << " -> " << (to + 8) << "\n";
                    move_list.add(Move(from, (to + n_shift), PAWN, no_promotion, false, true, false, false));
            }
        }
        
        U64 attacks = attack_tables.pawn_attacks[WHITE][from] & enemies;

        while(attacks != 0)
        {
            to = lsb(attacks);
            
            if(from >= a7)
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        if(game_state.en_passant != ep_none)
        {
            U8 ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(get_bit(attack_tables.pawn_attacks[WHITE][from], ep_square) != 0)
                //std::cout << "pawn enpassant " << from << " -> " << ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, no_promotion, true, false, true, false));
        }
        
        pop_bit(pawns, from);
    }
}

void Board::gen_black_pawn_moves(U64 occupancy, U64 enemies)
{
    U64 pawns = bb_pieces[BLACK][PAWN];
    while(pawns != 0)
    {
        U8 from = lsb(pawns);
        U8 to = from - s_shift;
        
        // forward moves
        if(to >= a1 && !get_bit(occupancy, to))  
        {
            if(from >= a2 && from <= h2) // promotion
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, false, false, false, false));
                // Double move from 7th rank
                if(from >= a7 && from <= h7 && !get_bit(occupancy, to - s_shift))
                {
                    //std::cout << "pawn " << from << " -> " << (to - 8) << "\n";
                    move_list.add(Move(from, (to - s_shift), PAWN, no_promotion, false, true, false, false));
                }
            }
        }
        
        // capture moves
        U64 attacks = attack_tables.pawn_attacks[BLACK][from] & enemies;
        while(attacks != 0)
        {
            to = lsb(attacks);
            if(from >= a2 && from <= h2) // promotion capture
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        // en passant
        if(game_state.en_passant != ep_none)
        {
            int ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(get_bit(attack_tables.pawn_attacks[BLACK][from], ep_square) != 0)
            {
                //std::cout << "pawn enpassant " << from << " -> " << ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, no_promotion, true, false, true, false));
            }
        }
        
        pop_bit(pawns, from);
    }
}

void Board::gen_knight_moves(U8 side, U64 enemies)
{
    U64 knights = (side == WHITE) ? bb_pieces[WHITE][KNIGHT] : bb_pieces[BLACK][KNIGHT];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(knights != 0)
    {
        U8 from = lsb(knights);

        U64 attacks = attack_tables.knight_attacks[from] & ~bb_side[side];
        
        while(attacks != 0)
        {
            U8 to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "knight " << from << " -> " << to << std::endl;
                move_list.add(Move(from, to, KNIGHT, no_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "knight " << from << " -> " << to << " captures " << enemy_captured << std::endl;
                move_list.add(Move(from, to, KNIGHT, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(knights, from);
    }
}

void Board::gen_king_moves(U8 side, U64 occupancy, U64 enemies)
{
    U64 king = (side == WHITE) ? bb_pieces[WHITE][KING] : bb_pieces[BLACK][KING];
    //std::string color = (side == WHITE) ? "white" : "black";

    U8 from = lsb(king);

    U64 attacks = attack_tables.king_attacks[from] & ~bb_side[side];

    while(attacks!= 0)
    {
        U8 to = lsb(attacks);

        if(!get_bit(enemies, to))
        {
            //std::cout << "king " << from << " -> " << to << std::endl;
            move_list.add(Move(from, to, KING, no_promotion, false, false, false, false));
        }
        else
        {
            //std::cout << "king " << from << " -> " << to << " captures " << enemy_captured << std::endl;
            move_list.add(Move(from, to, KING, no_promotion, true, false, false, false));
        }

        pop_bit(attacks, to);
    }

    // Castling moves
    gen_castling_moves(side, occupancy);
}

void Board::gen_castling_moves(U8 side, U64 occupancy) {
    if(side == WHITE) 
    {
        try_white_kingside_castle(occupancy);
        try_white_queenside_castle(occupancy);
    } 
    else 
    {
        try_black_kingside_castle(occupancy);
        try_black_queenside_castle(occupancy);
    }
}

void Board::try_white_kingside_castle(U64 occupancy)
{
    if ((wk & game_state.castling) == 0) return;
    if (!get_bit(bb_pieces[WHITE][ROOK], h1)) return; 
    if (get_bit(occupancy, f1) || get_bit(occupancy, g1)) return;
    if (is_square_attacked(e1, BLACK) || 
        is_square_attacked(f1, BLACK) || 
        is_square_attacked(g1, BLACK)) return;

    move_list.add(Move(e1, g1, KING, no_promotion, false, false, false, true));
}


void Board::try_white_queenside_castle(U64 occupancy)
{
    if ((wq & game_state.castling) == 0) return;
    if (!get_bit(bb_pieces[WHITE][ROOK], a1)) return; 
    if (get_bit(occupancy, b1) || get_bit(occupancy, c1) || get_bit(occupancy, d1)) return;
    if (is_square_attacked(d1, BLACK) || 
        is_square_attacked(c1, BLACK) || 
        is_square_attacked(e1, BLACK)) return;

    move_list.add(Move(e1, c1, KING, no_promotion, false, false, false, true));
}


void Board::try_black_kingside_castle(U64 occupancy)
{
    if ((bk & game_state.castling) == 0) return;
    if (!get_bit(bb_pieces[BLACK][ROOK], h8)) return; 
    if (get_bit(occupancy, f8) || get_bit(occupancy, g8)) return;
    if (is_square_attacked(e8, WHITE) || 
        is_square_attacked(f8, WHITE) || 
        is_square_attacked(g8, WHITE)) return;

    move_list.add(Move(e8, g8, KING, no_promotion, false, false, false, true));
}


void Board::try_black_queenside_castle(U64 occupancy)
{
    if ((bq & game_state.castling) == 0) return;
    if (!get_bit(bb_pieces[BLACK][ROOK], a8)) return; 
    if (get_bit(occupancy, b8) || get_bit(occupancy, c8) || get_bit(occupancy, d8)) return;
    if (is_square_attacked(d8, WHITE) || 
        is_square_attacked(c8, WHITE) || 
        is_square_attacked(e8, WHITE)) return;

    move_list.add(Move(e8, c8, KING, no_promotion, false, false, false, true));
}

void Board::gen_bishop_moves(U8 side, U64 occupancy, U64 enemies)
{
    U64 bishops = ((side == WHITE) ? bb_pieces[WHITE][BISHOP] : bb_pieces[BLACK][BISHOP]);

    while(bishops != 0)
    {
        U8 from = lsb(bishops);

        U64 attacks = attack_tables.get_bishop_attacks(from, occupancy);
        attacks &= ~bb_side[side];

        while(attacks != 0)
        {
            U8 to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "bishop " << (int)from << " -> " << (int)to << std::endl;
                move_list.add(Move(from, to, BISHOP, no_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "bishop " << (int)from << " -> " << (int)to << " captures " << std::endl;
                move_list.add(Move(from, to, BISHOP, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(bishops, from);
    }
}

void Board::gen_rook_moves(U8 side, U64 occupancy, U64 enemies)
{
    U64 rooks = (side == WHITE) ? bb_pieces[WHITE][ROOK] : bb_pieces[BLACK][ROOK];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(rooks != 0)
    {
        U8 from = lsb(rooks);

        U64 attacks = attack_tables.get_rook_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks != 0)
        {
            U8 to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "rook " << (int)from << " -> " << (int)to << std::endl;
                move_list.add(Move(from, to, ROOK, no_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "rook " << (int)from << " -> " << (int)to << " captures " << std::endl;
                move_list.add(Move(from, to, ROOK, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(rooks, from);
    }
}



void Board::gen_queen_moves(U8 side, U64 occupancy, U64 enemies)
{
    U64 queens = ((side == WHITE) ? bb_pieces[WHITE][QUEEN] : bb_pieces[BLACK][QUEEN]);
    std::string color = (side == WHITE) ? "white" : "black";

    while(queens != 0)
    {
        U8 from = lsb(queens);

        // Queen moves like both rook and bishop
        U64 attacks = attack_tables.get_rook_attacks(from, occupancy) | attack_tables.get_bishop_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks != 0)
        {
            U8 to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "queen " << from << (int)to << std::endl;
                move_list.add(Move(from, to, QUEEN, no_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "queen " << from << (int)to <<  std::endl;
                move_list.add(Move(from, to, QUEEN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(queens, from);
    }
}

bool Board::is_square_attacked(U8 square, U8 side)
{
    U64 board_occupancy = occupancy();

    if((attack_tables.pawn_attacks[side ^ 1][square] & bb_pieces[side][PAWN]) != 0) return true;
    
    if((attack_tables.knight_attacks[square] & bb_pieces[side][KNIGHT]) != 0) return true;
    
    if((attack_tables.king_attacks[square] & bb_pieces[side][KING]) != 0) return true;
    
    if((attack_tables.get_bishop_attacks(square, board_occupancy) & bb_pieces[side][BISHOP]) != 0) return true;
    
    if((attack_tables.get_rook_attacks(square, board_occupancy) & bb_pieces[side][ROOK])!= 0) return true;
    
    if((attack_tables.get_queen_attacks(square, board_occupancy) & bb_pieces[side][QUEEN])!= 0) return true;
    
    return false;
}
