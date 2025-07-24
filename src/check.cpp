#include "bitboards.hpp"
#include "types.hpp"
#include <iostream>

bool Board::alligned(U8 s1, U8 s2)
{   
    U8 s1_rank = rank_of(s1);
    U8 s2_rank = rank_of(s2);

    U8 s1_file = file_of(s1);
    U8 s2_file = file_of(s2);

    if(s1_rank == s2_rank || s1_file == s2_file) { return true; }
    
    if(abs(s1_rank - s2_rank) == abs(s1_file - s2_file)) { return true; }
    return false;
}

U64 Board::line_bb(U8 sq1, U8 sq2) 
{
    if (!alligned(sq1, sq2)) return 0ULL;

    return between_bb(sq1, sq2) | (1ULL << sq1) | (1ULL << sq2);
}


U64 Board::generate_bb_between(U8 from, U8 to)
{
    if(from == to || !alligned(to, from)) { return 0ULL; }

    U8 from_rank = rank_of(from);
    U8 to_rank = rank_of(to);

    U8 from_file = file_of(from);
    U8 to_file = file_of(to);

    int rank_dir = (to_rank > from_rank) ? 1 : (to_rank < from_rank) ? -1 : 0;
    int file_dir = (to_file > from_file) ? 1 : (to_file < from_file) ? -1 : 0;
    
    U64 line = 0ULL;
    int currentRank = from_rank + rank_dir;
    int currentFile = from_file + file_dir;


    while (currentRank != to_rank || currentFile != to_file) {
        int square = (currentRank * 8) + currentFile;
        line |= (1ULL << square);
        
        currentRank += rank_dir;
        currentFile += file_dir;
    }
    
    return line;
}

U64 Board::between_bb(U8 s1, U8 s2)
{
    return bb_between[s1][s2];
}

void Board::update_slider_blockers(U8 side) 
{
    U8 ksq = king_square(side);

    blockers_for_king[side] = 0ULL;
    pinners[~side] = 0ULL;    


    U64 snipers = 0ULL;


    U64 diagonal_pieces = get_piece(~side, BISHOP) | get_piece(~side, QUEEN);
    snipers |= diagonal_pieces & attack_tables.get_bishop_attacks(ksq, 0ULL);
    
    // straight line snipers (enemy rooks and queens)  
    U64 line_pieces = get_piece(~side, ROOK) | get_piece(~side, QUEEN);
    snipers |= line_pieces & attack_tables.get_rook_attacks(ksq, 0ULL);

    while(snipers != 0)
    {
        U8 sniper_sq = lsb(snipers);

        pop_bit(snipers, sniper_sq);

        U64 b = between_bb(ksq, sniper_sq) & occupancy();


        if(b && !more_than_one(b))
        {
            blockers_for_king[side] |= b;

            if(b & get_side(side))
            {
                pinners[~side] |= (1ULL << sniper_sq);
            }
        }
    }
    
}


void Board::update_check_info() 
{
    // update both pinners/blockers and checksquares
    update_slider_blockers(WHITE);
    update_slider_blockers(BLACK);
    
    U8 enemy_king_sq = king_square(opponent());
    U64 occ = occupancy();
    
    // clear all checksquares
    std::fill(checksquares.begin(), checksquares.end(), 0ULL);
    
    // calculate squares from which each piece type can give check
    checksquares[PAWN] = attack_tables.pawn_attacks[us()][enemy_king_sq];
    checksquares[KNIGHT] = attack_tables.knight_attacks[enemy_king_sq];
    checksquares[BISHOP] = attack_tables.get_bishop_attacks(enemy_king_sq, occ);
    checksquares[ROOK] = attack_tables.get_rook_attacks(enemy_king_sq, occ);
    checksquares[QUEEN] = attack_tables.get_queen_attacks(enemy_king_sq, occ);
}


bool Board::legal(Move move)
{

    U64 occ = occupancy();

    U8 side = game_state.active_color;
    U8 enemy = side ^ 1;
    U8 from = move.get_source();
    U8 to = move.get_target();
    U8 ksq = king_square(side);



    if(move.get_piece() == KING)
    {
        return !(is_square_attacked(to, enemy));
    }

    if(move.is_enpassant()){

        U8 captured_pawn_sq = enpassant_to_square(game_state.en_passant) + (side == WHITE ? -n_shift : s_shift);

        occ ^= (1ULL << from);           // remove moving pawn
        occ ^= (1ULL << captured_pawn_sq); // remove captured pawn  
        occ |= (1ULL << to);             // add moving pawn to destination

        const U64 bishop_attackers = bb_pieces[enemy][BISHOP] | bb_pieces[enemy][QUEEN];
        const U64 rook_attackers = bb_pieces[enemy][ROOK] | bb_pieces[enemy][QUEEN];

        return (!(attack_tables.get_bishop_attacks(ksq, occ) & bishop_attackers) 
        && !(attack_tables.get_rook_attacks(ksq, occ) & rook_attackers));
    }


    if (!(blockers_for_king[side] & (1ULL << from))) 
    {
        make_move(move);
        bool still_check = in_check(side);
        unmake_move();
        return !still_check;
    }



    return line_bb(from, to) & (1ULL << ksq);
}
