#include "bitboards.hpp"




void Board::make_move(const Move& move)
{   
    history.push(game_state);

    int side = game_state.active_color;
    int piece = move.get_piece();
    int source = move.get_source();
    int target = move.get_target();

    remove_piece(side, piece, source);
    put_piece(side, piece, target);

}

void Board::remove_piece(int side, int piece, int square)
{
    pop_bit(bb_pieces[side][piece],square);
    pop_bit(bb_side[side],square);
    piece_list[square] = NONE; 
    game_state.zobrist_key ^= zobrist_randoms.piece(side, piece, square);
}

void Board:: put_piece(int side, int piece, int square)
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
                std::cout << from << to << "q\n";
                std::cout << from << to << "r\n"; 
                std::cout << from << to << "n\n";
                std::cout << from << to << "b\n";
            }
            else
            {
                std::cout << from << to << "\n"; 
                
                if(from >= a2 && from <= h2 && !get_bit(occupancy, to + 8))
                    std::cout << from << (to + 8) << "\n";
            }
        }
        
        U64 attacks = attack_tables.pawn_attacks[WHITE][from] & enemies;

        while(attacks)
        {
            to = lsb(attacks);
            
            if(from >= a7)
            {
                std::cout << from << to << "q\n";
                std::cout << from << to << "r\n";
                std::cout << from << to << "n\n"; 
                std::cout << from << to << "b\n";
            }
            else
                std::cout << from << to << "\n";
                
            pop_bit(attacks, to);
        }
        if(game_state.en_passant != 16)
        {
            int ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(ep_square != -1 && get_bit(attack_tables.pawn_attacks[WHITE][from], ep_square))
                std::cout << from << ep_square << "\n";
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
            if(from >= a2 && from <= h2) 
            {
                std::cout << from << to << "q\n";
                std::cout << from << to << "r\n";
                std::cout << from << to << "n\n";
                std::cout << from << to << "b\n";
            }
            else
            {
                std::cout << from << to << "\n";
                // Double move from 7th rank
                if(from >= a7 && from <= h7 && !get_bit(occupancy, to - 8))
                    std::cout << from << (to - 8) << "\n";
            }
        }
        
        // Capture moves
        U64 attacks = attack_tables.pawn_attacks[BLACK][from] & enemies;
        while(attacks)
        {
            to = lsb(attacks);
            if(from >= a2 && from <= h2) 
            {
                std::cout << from << to << "q\n";
                std::cout << from << to << "r\n";
                std::cout << from << to << "n\n";
                std::cout << from << to << "b\n";
            }
            else
                std::cout << from << to << "\n";
            pop_bit(attacks, to);
        }
        
        if(game_state.en_passant != 16)
        {
            int ep_square = game_state.enpassant_to_square(game_state.en_passant);
            if(ep_square != -1 && get_bit(attack_tables.pawn_attacks[BLACK][from], ep_square))
                std::cout << from << ep_square << "\n";
        }
        
        pop_bit(pawns, from);
    }
}

void Board::gen_knight_moves(int side, U64 enemies)
{
    U64 knights;
    knights = (side == WHITE) ? bb_pieces[WHITE][KNIGHT] : bb_pieces[BLACK][KNIGHT];

    while(knights)
    {
        int from = lsb(knights);

        U64 attacks = attack_tables.knight_attacks[from] & ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                std::cout << "knight quiet move " << from << to << std::endl; 
            }
            else
            {
                int enemy_captured = piece_list[to];
                std::cout << "knight capturing " << enemy_captured << " " << from << to << std::endl;
            }
            pop_bit(attacks, to);
        }
        pop_bit(knights, from);
    }
    
}

void Board::gen_king_moves(int side, U64 occupancy, U64 enemies){
    U64 king;
    king = (side == WHITE) ? bb_pieces[WHITE][KING] : bb_pieces[BLACK][KING];

    int from = lsb(king);

    U64 attacks = attack_tables.king_attacks[from] & ~bb_side[side];

    while(attacks)
    {
        int to = lsb(attacks);

        if(!get_bit(enemies, to))
        {
            std::cout << "king quiet move " << from << to << std::endl; 
        }
        else
        {
            int enemy_captured = piece_list[to];
            std::cout << "king capturing " << enemy_captured << " " << from << to << std::endl;
        }

        pop_bit(attacks, to);
    }


    if(side == WHITE)
    {
        if(wk & game_state.castling)
        {
            if(!get_bit(occupancy, g1) && !get_bit(occupancy, f1))
            {
                if(!is_square_attacked(g1,BLACK) && !is_square_attacked(e1,BLACK) && !is_square_attacked(f1,BLACK))
                {
                    std::cout << "white king castling king side" << std::endl;
                }
            }

        }

        if(wq & game_state.castling)
        {
            if(!get_bit(occupancy, b1) && !get_bit(occupancy, c1) && !get_bit(occupancy, d1))
            {
                if( !is_square_attacked(e1,BLACK) && !is_square_attacked(c1,BLACK) && !is_square_attacked(d1,BLACK) )
                {
                    std::cout << "white king castling queen side" << std::endl;
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
                    std::cout << "black king castling king side" << std::endl;
                }
            }

        }

        if(wq & game_state.castling)
        {
            if(!get_bit(occupancy, b8) && !get_bit(occupancy, c8) && !get_bit(occupancy, d8))
            {
                if( !is_square_attacked(e8,WHITE) && !is_square_attacked(c8,WHITE) && !is_square_attacked(d8,WHITE) )
                {
                    std::cout << "black king castling queen side" << std::endl;
                }
            }

        }
    }
}

void Board::gen_bishop_moves(int side, U64 occupancy, U64 enemies)
{
    U64 bishops;
    bishops = (side == WHITE) ? bb_pieces[WHITE][BISHOP] : bb_pieces[BLACK][BISHOP];

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
                std::cout << "bishop quiet move " << from << to << std::endl; 
            }
            else
            {
                int enemy_captured = piece_list[to];
                std::cout << "bishop capturing " << enemy_captured << " " << from << to << std::endl;
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
                std::cout << "rook quiet move " << from << to << std::endl; 
            }
            else
            {
                int enemy_captured = piece_list[to];
                std::cout << "rook capturing " << enemy_captured << " " << from << to << std::endl;
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

    while(queens)
    {
        int from = lsb(queens);

        U64 attacks = attack_tables.get_rook_attacks(from, occupancy);
        attacks &= ~bb_side[side];
        
        while(attacks)
        {
            int to = lsb(attacks);

            if(!get_bit(enemies, to))
            {
                std::cout << "queen quiet move " << from << to << std::endl; 
            }
            else
            {
                int enemy_captured = piece_list[to];
                std::cout << "queen capturing " << enemy_captured << " " << from << to << std::endl;
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
