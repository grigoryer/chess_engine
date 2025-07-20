/*
void Board::gen_bishop_moves(U8 side, U64 occupancy, U64 enemies)
{
    U64 bishops = (side == WHITE) ? bb_pieces[WHITE][BISHOP] : bb_pieces[BLACK][BISHOP];
    //std::string color = (side == WHITE) ? "white" : "black";

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
                std::cout << "bishop " << (int)from << " -> " << (int)to << std::endl;
                move_list.add(Move(from, to, BISHOP, no_promotion, false, false, false, false));
            }
            else
            {
                std::cout << "bishop " << (int)from << " -> " << (int)to << " captures " << std::endl;
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
                std::cout << "rook " << (int)from << " -> " << (int)to << std::endl;
                move_list.add(Move(from, to, ROOK, no_promotion, false, false, false, false));
            }
            else
            {
                std::cout << "rook " << (int)from << " -> " << (int)to << " captures " << std::endl;
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
                std::cout << "queen " << from << (int)to << std::endl;
                move_list.add(Move(from, to, QUEEN, no_promotion, false, false, false, false));
            }
            else
            {
                std::cout << "queen " << from << (int)to <<  std::endl;
                move_list.add(Move(from, to, QUEEN, no_promotion, true, false, false, false));
            }
            pop_bit(attacks, to);
        }
        pop_bit(queens, from);
    }
}
*/