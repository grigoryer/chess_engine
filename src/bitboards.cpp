#include "bitboards.hpp"
#include "zobrist.hpp"

Board::Board()
{
    init_piece_list();
    game_state.zobrist_key = init_zobrist_key();
}


GameState::GameState() 
: active_color(WHITE)
, castling(0)
, half_move_clock(0)
, en_passant(0)
, fullmove_number(1)
, zobrist_key(0)
, phase_value(0)
{  }




History::History()
    : count(0)
{  }


void print_board(U64 bitboard) 
{
    std::cout << "\n";
    const U64 LAST_BIT = 63;

    for (int rank = 0; rank < 8; ++rank) 
    {
        std::cout << 8 - rank << "  ";
        for (int file = 7; file >= 0; --file) 
        {
            U64 mask = 1ULL << (LAST_BIT - (rank * 8 + file));
            char c = (bitboard & mask) ? '1' : '.';
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "   a b c d e f g h\n\n";
    std::cout << "#: " << bitboard << '\n';
}



U64 Board::init_zobrist_key() const 
{
    U64 key = 0;

    for (int side = 0; side < NUM_SIDES; side++) 
    {
        for (int piece = 0; piece < NUM_PIECES; piece++) 
        {
            U64 bitboard = bb_pieces[side][piece];
            
            while (bitboard) 
            {
                int square = lsb(bitboard);
                key ^= zobrist_randoms.piece(side, piece, square);
                pop_bit(bitboard,square);
            }
        }
    }

    key ^= zobrist_randoms.castling(game_state.castling);
    key ^= zobrist_randoms.side(game_state.active_color);
    key ^= zobrist_randoms.en_passant(game_state.en_passant);

    return key;    
}

void Board::init_piece_list()
{
    
    for(int piece_type = KING; piece_type <= PAWN; piece_type++)
    {  
        U64 bb_w = bb_pieces[WHITE][piece_type];
        while(bb_w)
        {
            int square = lsb(bb_w);
            piece_list[square] = piece_type;
            pop_bit(bb_w, square);
        }
        
        U64 bb_b = bb_pieces[BLACK][piece_type];
        while(bb_b)
        {
            int square = lsb(bb_b);
            piece_list[square] = piece_type;
            pop_bit(bb_b, square);
        }
    }
}

void Board::init()
{
    init_pieces_per_side_bitboard();
    init_piece_list();
    game_state.zobrist_key = init_zobrist_key();
}

void Board::init_pieces_per_side_bitboard()
{
    for(int piece = 0; piece < NUM_PIECES; piece++)
    {
        bb_side[WHITE] |= bb_pieces[WHITE][piece];
        bb_side[BLACK] |= bb_pieces[BLACK][piece];
    }
}



U64 Board::occupancy()
{
    return bb_side[WHITE] | bb_side[BLACK];
}





void History::push(GameState g)
{
    list[count] = g;
    count++;
}

std::optional<GameState> History::pop() 
{
    if (count > 0) {
        count -= 1;
        return list[count];
    } else {
        return std::nullopt;
    }
}

void History::clear()
{
    count = 0;
}

size_t History::len()
{
    return count;
}

GameState& History::get_ref(size_t index)
{
    return list[index];
}