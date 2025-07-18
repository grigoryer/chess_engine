#include "bitboards.hpp"




void Board::make_move(const Move move)
{   
    game_state.move = move;
    history.push(game_state);

    int side = us();
    int piece = move.get_piece();
    int source = move.get_source();
    int target = move.get_target();

    if(move.is_promotion() != 0) 
    {
        int promoted_piece = promotion_support();
        remove_piece(side, piece, source);
        put_piece(side, promoted_piece, target);
    } 
    else 
    {
        move_piece(side, piece, source, target);
    }

    move.print_move(side);
    move_support(side, piece, target);
}

void Board::move_support(int side, int piece, int target)
{

    if(move.is_castling()) { castling_support(target); }
    if(move.is_enpassant()) { enpassant_support(); }
    if(move.is_double()) { double_support(target); }

    if(move.is_capture() || move.is_promotion() || piece == PAWN) 
    { 
        game_state.half_move_clock = 0; 
    }

    if(piece == ROOK) 
    { 
        update_castling_permissions(game_state.castling & ~(side == WHITE ? 3 : 12));
    }

    if(piece == KING) 
    {
        update_castling_permissions(game_state.castling & ~(side == WHITE ? 3 : 12));
    }

    if(!move.is_double()) { clear_epsquare(); }

    move_list.clear();
    swap_sides();
}


int Board::promotion_support()
{
    switch(move.get_promoted())
    {
        case(1): return KNIGHT;
        case(2): return BISHOP;
        case(4): return ROOK;
        case(8): return QUEEN;
        default: return QUEEN; // fallback
    }
}

void Board::castling_support(int king_target)
{
    if(king_target == g1) 
    { 
        remove_piece(WHITE, ROOK, h1);
        put_piece(WHITE, ROOK, f1);
        update_castling_permissions(game_state.castling & ~3);
    }
    else if(king_target == c1) 
    { 
        remove_piece(WHITE, ROOK, a1);
        put_piece(WHITE, ROOK, d1);
        update_castling_permissions(game_state.castling & ~3);
    }
    else if(king_target == g8) 
    { 
        remove_piece(BLACK, ROOK, h8);
        put_piece(BLACK, ROOK, f8);
        update_castling_permissions(game_state.castling & ~12);
    }
    else if(king_target == c8) 
    { 
        remove_piece(BLACK, ROOK, a8);
        put_piece(BLACK, ROOK, d8);
        update_castling_permissions(game_state.castling & ~12);
    }
}

void Board::enpassant_support()
{
    int target_square = 0;
    int ep = game_state.en_passant;
    target_square = ep + (ep < 7 ? 24 : 32);
    remove_piece(opponent(), PAWN, target_square);
}

void Board::double_support(int target)
{
    
    set_epsquare(target + (us() == WHITE ? -8 : 8));
    game_state.half_move_clock = 0;
}

void Board::move_piece(int side, int piece, int from, int to)
{
    remove_piece(side, piece, from);
    put_piece(side, piece, to);
}

void Board::remove_piece(int side, int piece, int square)
{
    pop_bit(bb_pieces[side][piece],square);
    pop_bit(bb_side[side],square);
    piece_list[square] = NONE; 
    game_state.zobrist_key ^= zobrist_randoms.piece(side, piece, square);
}

void Board::put_piece(int side, int piece, int square)
{
    set_bit(bb_pieces[side][piece],square);
    set_bit(bb_side[side],square);
    int piece_type = !side ? piece + 1 : piece + 7;
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

void Board::set_epsquare(int ep_square)
{
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
    game_state.en_passant = ep_square;
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
}

void Board::clear_epsquare()
{
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
    game_state.en_passant = 16;
    game_state.zobrist_key ^= zobrist_randoms.en_passant(game_state.en_passant);
}

void Board::gen_moves(int side)
{
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
    int side = WHITE;
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
    move_list.clear();
    int side = BLACK;
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
    while(pawns)
    {
        int from = lsb(pawns);
        int to = from + 8;

        if(to <= h8 && !get_bit(occupancy, to))
        {
            if(from >= a7) // Promotion
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, 8, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, 4, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, 1, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, 2, false, false, false, false));

            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, false, false, false, false, false));
                
                if(from >= a2 && from <= h2 && !get_bit(occupancy, to + 8))
                  //std::cout << "pawn " << from << " -> " << (to + 8) << "\n";
                    move_list.add(Move(from, (to + 8), PAWN, false, false, true, false, false));
            }
        }
        
        U64 attacks = attack_tables.pawn_attacks[WHITE][from] & enemies;

        while(attacks)
        {
            to = lsb(attacks);
            
            if(from >= a7)
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, 8, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, 4, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, 1, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, 2, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        if(game_state.en_passant != 16)
        {
            int ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(ep_square != -1 && get_bit(attack_tables.pawn_attacks[WHITE][from], ep_square))
                //std::cout << "pawn enpassant " << from << " -> " << ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, false, true, false, true, false));
        }
        
        pop_bit(pawns, from);
    }
}

void Board::gen_black_pawn_moves(U64 occupancy, U64 enemies)
{
    U64 pawns = bb_pieces[BLACK][PAWN];
    while(pawns)
    {
        int from = lsb(pawns);
        int to = from - 8;
        
        // Forward moves
        if(to >= a1 && !get_bit(occupancy, to))  
        {
            if(from >= a2 && from <= h2) // Promotion
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, 8, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, 4, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, 1, false, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, 2, false, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, false, false, false, false, false));
                // Double move from 7th rank
                if(from >= a7 && from <= h7 && !get_bit(occupancy, to - 8))
                {
                    //std::cout << "pawn " << from << " -> " << (to - 8) << "\n";
                    move_list.add(Move(from, (to - 8), PAWN, false, false, true, false, false));
                }
            }
        }
        
        // Capture moves
        U64 attacks = attack_tables.pawn_attacks[BLACK][from] & enemies;
        while(attacks)
        {
            to = lsb(attacks);
            if(from >= a2 && from <= h2) // Promotion capture
            {
                //std::cout << "pawn " << from << " -> " << to << "q\n";
                move_list.add(Move(from, to, PAWN, 8, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "r\n";
                move_list.add(Move(from, to, PAWN, 4, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "n\n";
                move_list.add(Move(from, to, PAWN, 1, true, false, false, false));
                //std::cout << "pawn " << from << " -> " << to << "b\n";
                move_list.add(Move(from, to, PAWN, 2, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << from << " -> " << to << "\n";
                move_list.add(Move(from, to, PAWN, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        
        if(game_state.en_passant != 16)
        {
            int ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(ep_square != -1 && get_bit(attack_tables.pawn_attacks[BLACK][from], ep_square))
            {
                //std::cout << "pawn enpassant " << from << " -> " << ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, false, true, false, true, false));
            }
        }
        
        pop_bit(pawns, from);
    }
}

void Board::gen_knight_moves(int side, U64 enemies)
{
    U64 knights;
    knights = (side == WHITE) ? bb_pieces[WHITE][KNIGHT] : bb_pieces[BLACK][KNIGHT];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(knights)
    {
        int from = lsb(knights);

        U64 attacks = attack_tables.knight_attacks[from] & ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "knight " << from << " -> " << to << std::endl;
                move_list.add(Move(from, to, KNIGHT, false, false, false, false, false));
            }
            else
            {
                int enemy_captured = piece_list[to];
                //std::cout << "knight " << from << " -> " << to << " captures " << enemy_captured << std::endl;
                move_list.add(Move(from, to, KNIGHT, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(knights, from);
    }
}

void Board::gen_king_moves(int side, U64 occupancy, U64 enemies)
{
    U64 king;
    king = (side == WHITE) ? bb_pieces[WHITE][KING] : bb_pieces[BLACK][KING];
    //std::string color = (side == WHITE) ? "white" : "black";

    int from = lsb(king);

    U64 attacks = attack_tables.king_attacks[from] & ~bb_side[side];

    while(attacks)
    {
        int to = lsb(attacks);

        if(!get_bit(enemies, to))
        {
            //std::cout << "king " << from << " -> " << to << std::endl;
            move_list.add(Move(from, to, KING, false, false, false, false, false));
        }
        else
        {
            int enemy_captured = piece_list[to];
            //std::cout << "king " << from << " -> " << to << " captures " << enemy_captured << std::endl;
            move_list.add(Move(from, to, KING, false, true, false, false, false));
        }

        pop_bit(attacks, to);
    }

    // Castling moves
    if(side == WHITE)
    {
        if(wk & game_state.castling)
        {
            if(!get_bit(occupancy, g1) && !get_bit(occupancy, f1))
            {
                if(!is_square_attacked(g1,BLACK) && !is_square_attacked(e1,BLACK) && !is_square_attacked(f1,BLACK))
                {
                    //std::cout << "king " << e1 << " -> " << g1 << std::endl;
                    move_list.add(Move(e1, g1, KING, false, false, false, false, true));
                }
            }
        }

        if(wq & game_state.castling)
        {
            if(!get_bit(occupancy, b1) && !get_bit(occupancy, c1) && !get_bit(occupancy, d1))
            {
                if(!is_square_attacked(e1,BLACK) && !is_square_attacked(c1,BLACK) && !is_square_attacked(d1,BLACK))
                {
                    //std::cout << "king " << e1 << " -> " << c1 << std::endl;
                    move_list.add(Move(e1, c1, KING, false, false, false, false, true));
                }
            }
        }
    }
    else
    {
        if(bk & game_state.castling)
        {
            if(!get_bit(occupancy, g8) && !get_bit(occupancy, f8))
            {
                if(!is_square_attacked(g8,WHITE) && !is_square_attacked(e8,WHITE) && !is_square_attacked(f8,WHITE))
                {
                    //std::cout << "king " << e8 << " -> " << g8 << std::endl;
                    move_list.add(Move(e8, g8, KING, false, false, false, false, true));
                }
            }
        }

        if(bq & game_state.castling)
        {
            if(!get_bit(occupancy, b8) && !get_bit(occupancy, c8) && !get_bit(occupancy, d8))
            {
                if(!is_square_attacked(e8,WHITE) && !is_square_attacked(c8,WHITE) && !is_square_attacked(d8,WHITE))
                {
                    //std::cout << "king " << e8 << " -> " << c8 << std::endl;
                    move_list.add(Move(e8, c8, KING, false, false, false, false, true));
                }
            }
        }
    }
}

void Board::gen_bishop_moves(int side, U64 occupancy, U64 enemies)
{
    U64 bishops;
    bishops = (side == WHITE) ? bb_pieces[WHITE][BISHOP] : bb_pieces[BLACK][BISHOP];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(bishops)
    {
        int from = lsb(bishops);

        U64 attacks = attack_tables.get_bishop_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "bishop " << from << " -> " << to << std::endl;
                move_list.add(Move(from, to, BISHOP, false, false, false, false, false));
            }
            else
            {
                int enemy_captured = piece_list[to];
                //std::cout << "bishop " << from << " -> " << to << " captures " << enemy_captured << std::endl;
                move_list.add(Move(from, to, BISHOP, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(bishops, from);
    }
}

void Board::gen_rook_moves(int side, U64 occupancy, U64 enemies)
{
    U64 rooks;
    rooks = (side == WHITE) ? bb_pieces[WHITE][ROOK] : bb_pieces[BLACK][ROOK];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(rooks)
    {
        int from = lsb(rooks);

        U64 attacks = attack_tables.get_rook_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "rook " << from << " -> " << to << std::endl;
                move_list.add(Move(from, to, ROOK, false, false, false, false, false));
            }
            else
            {
                int enemy_captured = piece_list[to];
                //std::cout << "rook " << from << " -> " << to << " captures " << enemy_captured << std::endl;
                move_list.add(Move(from, to, ROOK, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(rooks, from);
    }
}

void Board::gen_queen_moves(int side, U64 occupancy, U64 enemies)
{
    U64 queens;
    queens = (side == WHITE) ? bb_pieces[WHITE][QUEEN] : bb_pieces[BLACK][QUEEN];
    //std::string color = (side == WHITE) ? "white" : "black";

    while(queens)
    {
        int from = lsb(queens);

        // Queen moves like both rook and bishop
        U64 attacks = attack_tables.get_rook_attacks(from, occupancy) | attack_tables.get_bishop_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                //std::cout << "queen " << from << " -> " << to << std::endl;
                move_list.add(Move(from, to, QUEEN, false, false, false, false, false));
            }
            else
            {
                int enemy_captured = piece_list[to];
                //std::cout << "queen " << from << " -> " << to << " captures " << enemy_captured << std::endl;
                move_list.add(Move(from, to, QUEEN, false, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(queens, from);
    }
}

bool Board::is_square_attacked(int square, int side)
{
    U64 board_occupancy = occupancy();

    if(attack_tables.pawn_attacks[side ^ 1][square] & bb_pieces[side][PAWN]) return true;
    
    if(attack_tables.knight_attacks[square] & bb_pieces[side][KNIGHT]) return true;
    
    if(attack_tables.king_attacks[square] & bb_pieces[side][KING]) return true;
    
    if(attack_tables.get_bishop_attacks(square, board_occupancy) & bb_pieces[side][BISHOP]) return true;
    
    if(attack_tables.get_rook_attacks(square, board_occupancy) & bb_pieces[side][ROOK]) return true;
    
    if(attack_tables.get_queen_attacks(square, board_occupancy) & bb_pieces[side][QUEEN]) return true;
    
    return false;
}








