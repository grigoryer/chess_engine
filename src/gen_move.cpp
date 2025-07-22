#include "bitboards.hpp"
#include "types.hpp"
#include <iostream>

void Board::gen_moves(U8 side)
{
    move_list.clear();

    if(king_sqaure(side) < a1 || king_sqaure(side) > h8) { std::cout << "King Died"; return; }

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
    gen_white_pawn_moves(occupied, enemies);
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
    gen_black_pawn_moves(occupied, enemies);
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
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, false, false, false, false));
                if(from >= a2 && from <= h2 && !get_bit(occupancy, to + n_shift))
                {
                    //std::cout << "pawn " << (int)from << " -> " << (int)(to + 8) << "\n";
                    move_list.add(Move(from, (to + n_shift), PAWN, no_promotion, false, true, false, false));
                }
            }
        }
        U64 attacks = attack_tables.pawn_attacks[WHITE][from] & enemies;
        while(attacks != 0)
        {
            to = lsb(attacks);
            if(from >= a7)
            {
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        if(game_state.en_passant != ep_none)
        {
            U8 ep_square = enpassant_to_square(game_state.en_passant);
            if(get_bit(attack_tables.pawn_attacks[WHITE][from], ep_square))
            {
                //std::cout << "pawn enpassant " << (int)from << " -> " << (int)ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, no_promotion, false, false, true, false));
            }
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
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, false, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, false, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, false, false, false, false));
                // Double move from 7th rank
                if(from >= a7 && from <= h7 && !get_bit(occupancy, to - s_shift))
                {
                    //std::cout << "pawn " << (int)from << " -> " << (int)(to - 8) << "\n";
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
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "q\n";
                move_list.add(Move(from, to, PAWN, q_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "r\n";
                move_list.add(Move(from, to, PAWN, r_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "n\n";
                move_list.add(Move(from, to, PAWN, b_promotion, true, false, false, false));
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "b\n";
                move_list.add(Move(from, to, PAWN, n_promotion, true, false, false, false));
            }
            else
            {
                //std::cout << "pawn " << (int)from << " -> " << (int)to << "\n";
                move_list.add(Move(from, to, PAWN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        // en passant
        if(game_state.en_passant != ep_none)
        {
            U8 ep_square = enpassant_to_square(game_state.en_passant);
            if(get_bit(attack_tables.pawn_attacks[BLACK][from], ep_square))
            {
                //std::cout << "pawn enpassant " << (int)from << " -> " << (int)ep_square << "\n";
                move_list.add(Move(from, ep_square, PAWN, no_promotion, false, false, true, false));
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
    if ((wk & game_state.castling) == 0) { return; }
    if (!get_bit(bb_pieces[WHITE][ROOK], h1)) { return; }
    if (get_bit(occupancy, f1) || get_bit(occupancy, g1)) { return; }
    if (is_square_attacked(e1, BLACK) || 
        is_square_attacked(f1, BLACK) || 
        is_square_attacked(g1, BLACK)) { return; }

    move_list.add(Move(e1, g1, KING, no_promotion, false, false, false, true));
}

void Board::try_white_queenside_castle(U64 occupancy)
{
    if ((wq & game_state.castling) == 0) { return; }
    if (!get_bit(bb_pieces[WHITE][ROOK], a1)) { return; }  
    if (get_bit(occupancy, b1) || get_bit(occupancy, c1) || get_bit(occupancy, d1)) { return; }
    if (is_square_attacked(d1, BLACK) || 
        is_square_attacked(c1, BLACK) || 
        is_square_attacked(e1, BLACK)) { return; }

    move_list.add(Move(e1, c1, KING, no_promotion, false, false, false, true));
}

void Board::try_black_kingside_castle(U64 occupancy)
{
    if ((bk & game_state.castling) == 0) { return; }
    if (!get_bit(bb_pieces[BLACK][ROOK], h8)) { return; }
    if (get_bit(occupancy, f8) || get_bit(occupancy, g8)) { return; }
    if (is_square_attacked(e8, WHITE) || 
        is_square_attacked(f8, WHITE) || 
        is_square_attacked(g8, WHITE)) { return; }

    move_list.add(Move(e8, g8, KING, no_promotion, false, false, false, true));
}

void Board::try_black_queenside_castle(U64 occupancy)
{
    if ((bq & game_state.castling) == 0) { return; }
    if (!get_bit(bb_pieces[BLACK][ROOK], a8)) { return; }
    if (get_bit(occupancy, b8) || get_bit(occupancy, c8) || get_bit(occupancy, d8)) { return; }
    if (is_square_attacked(d8, WHITE) || 
        is_square_attacked(c8, WHITE) || 
        is_square_attacked(e8, WHITE)) { return; }

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

    while(queens != 0)
    {
        U8 from = lsb(queens);

        // queen moves like both rook and bishop
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

    if((attack_tables.pawn_attacks[side ^ 1][square] & bb_pieces[side][PAWN]) != 0) { return true; } 
    
    if((attack_tables.knight_attacks[square] & bb_pieces[side][KNIGHT]) != 0) { return true; } 
    
    if((attack_tables.king_attacks[square] & bb_pieces[side][KING]) != 0) { return true; } 
    
    if((attack_tables.get_bishop_attacks(square, board_occupancy) & bb_pieces[side][BISHOP]) != 0) { return true; } 
    
    if((attack_tables.get_rook_attacks(square, board_occupancy) & bb_pieces[side][ROOK])!= 0) { return true; } 
    
    if((attack_tables.get_queen_attacks(square, board_occupancy) & bb_pieces[side][QUEEN])!= 0) { return true; } 
    return false;
}

bool Board::is_check(U8 side)
{
    U8 king_square = king_sqaure(side);
    return is_square_attacked(king_square,opponent());
}
