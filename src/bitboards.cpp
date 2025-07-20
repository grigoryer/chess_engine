#include "bitboards.hpp"
#include "types.hpp"
#include <sstream>
#include <cstring>  
#include <iostream>

/*=========================
    DEBUG FUNCTIONS
=========================*/

void print_board(U64 bitboard) 
{
    std::cout << "\n";
    const U64 LAST_BIT = 63;

    for (int rank = 0; rank <= MAX_RANK; ++rank) 
    {
        std::cout << BOARD_LENGTH - rank << "  ";

        for (int file = MAX_FILE; file >= 0; --file) 
        {
            U64 mask = 1ULL << (LAST_BIT - (rank * BOARD_LENGTH + file));
            char c = (bitboard & mask) ? '1' : '.';
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "   a b c d e f g h\n\n";
    std::cout << "#: " << bitboard << '\n';
}

void Board::print_piece_list() 
{

    constexpr std::array<char, 13> piece_to_char = {
        '.', 'K', 'Q', 'R', 'B', 'N', 'P',
        'k', 'q', 'r', 'b', 'n', 'p'
    };
    
    std::cout << "\n";

    for (int rank = MAX_RANK; rank >= 0; rank--) 
    {  
        std::cout << rank + 1 << "  ";
        for (int file = 0; file <= MAX_FILE; file++) 
        {
            int square = (rank * BOARD_LENGTH) + file;
            char piece_char = piece_to_char[piece_list[square]];
            std::cout << piece_char << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n   a b c d e f g h\n";
}

/*=========================
    BOARD STRUCT
=========================*/

Board::Board()
{
    init();
}

void Board::fen_parser(const std::string& fen)
{
    static constexpr int fen_position = 0;
    static constexpr int fen_color = 1;
    static constexpr int fen_castling = 2;
    static constexpr int fen_en_passant = 3;
    static constexpr int fen_half_move = 4;
    static constexpr int fen_full_move = 5;


    std::vector<std::string> parts;
    std::istringstream iss(fen);
    std::string word;

    while(iss >> word) {
        parts.push_back(word);
    }

    //split up string into parts
    int rank = 0; 
    int file = 0;

    std::string position = parts[fen_position];
    std::string color = parts[fen_color];
    std::string castling = parts[fen_castling];
    std::string en_passant = parts[fen_en_passant];
    std::string half_move = parts[fen_half_move];
    std::string move = parts[fen_full_move];

    // handle the first position part of the fen string and add bits where specified.
    for(int i = 0; i < position.length(); i++)
    {
        char ch = position[i];

        if(ch == '/')
        {
            rank++;
            file = 0;
        }
        else if (std::isdigit(ch) != 0) 
        {
            file += (ch - '0');
        }
        else
        {
            U8 isWhite = isupper(ch) ? 0 : 1;
            int square = ((MAX_RANK - rank) * BOARD_LENGTH) + file;
            ch = (char)tolower(ch);

            switch(ch)
            {
                case 'k': set_bit(bb_pieces[isWhite][KING], square); break;
                case 'q': set_bit(bb_pieces[isWhite][QUEEN], square); break;
                case 'r': set_bit(bb_pieces[isWhite][ROOK], square); break;
                case 'b': set_bit(bb_pieces[isWhite][BISHOP], square); break;
                case 'n': set_bit(bb_pieces[isWhite][KNIGHT], square); break;
                case 'p': set_bit(bb_pieces[isWhite][PAWN], square); break;
            }
            file++;   
        }
    }

    game_state.active_color = (color == "w" ) ?  WHITE : BLACK;

    game_state.castling = no_castling;
    // create the castling int
    for(int i = 0; i < castling.length(); i++)
    {
        char ch = castling[i];
        switch(ch) 
        {
            case 'K': game_state.castling |= wk; break;
            case 'Q': game_state.castling |= wq; break;
            case 'k': game_state.castling |= bk; break;
            case 'q': game_state.castling |= bq; break;
            case '-': break;
        }
    }

    //parse en passant if any.
    if(en_passant != "-")
    {
        int file = en_passant[0] - 'a';
        int rank = en_passant[1] - '0';

        if(rank == ep_rank_white) 
        {
            game_state.en_passant = file; // 0-7
        }
        else if(rank == ep_rank_black) 
        {
            game_state.en_passant = BOARD_LENGTH + file; 
        }
    }
    else
    {
        game_state.en_passant = ep_none; //none value
    }

    if(half_move == "-" || parts.size() < fen_half_move){game_state.half_move_clock = 0;}
    else {game_state.half_move_clock = std::stoi(half_move);}

    if(move == "-" || parts.size() < fen_full_move){game_state.fullmove_number = 0;}
    else{game_state.fullmove_number = std::stoi(move);}
}


void Board::init()
{
    fen_parser(STARTING_FEN);
    init_pieces_per_side_bitboard();
    init_piece_list();
    game_state.zobrist_key = init_zobrist_key();
}

U64 Board::init_zobrist_key() 
{
    U64 key = 0;

    for (int side = 0; side < NUM_SIDES; side++) 
    {
        for (int piece = 0; piece < NUM_PIECES; piece++) 
        {
            U64 bitboard = bb_pieces[side][piece];
            
            while (bitboard != 0) 
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
        while(bb_w != 0)
        {
            int square = lsb(bb_w);
            piece_list[square] = (PieceList)(K + piece_type);
            pop_bit(bb_w, square);
        }
        
        U64 bb_b = bb_pieces[BLACK][piece_type];
        while(bb_b != 0)
        {
            int square = lsb(bb_b);
            piece_list[square] = (PieceList)(k + piece_type);
            pop_bit(bb_b, square);
        }
    }
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

U8 Board::us()
{
    return game_state.active_color;
}

U8 Board::opponent()
{
    return game_state.active_color ^ 1;
}

void Board::reset_board()
{
    bb_pieces.fill(std::array<U64, NUM_PIECES>{0ULL});
    bb_side.fill(0ULL);
    piece_list.fill(NONE);

    game_state.clear();
    history.clear();
}

U8 Board::king_sqaure(U8 side)
{
    return lsb(bb_pieces[side][KING]);
}

U64& Board::get_piece(U8 side, U8 piece)
{
    return bb_pieces[side][piece];
}

U64& Board::get_side(U8 side)
{
     return bb_side[side];
}

bool Board::has_bishop_pair(U8 side) 
{
    U64 bb_bishop = get_piece(side, BISHOP);

    if(bits_in_bitboard(bb_bishop) < 2) { return false; }

    int count_dark = 0;
    int count_light = 0;

    while(bb_bishop != 0)
    {
        int square = lsb(bb_bishop);
        if(is_white_square(square)) { count_light++; }
        else {count_dark++; }
        pop_bit(bb_bishop, square);
    }

    return (count_dark > 0 && count_light > 0);
}

bool Board::is_white_square(U8 square)
{
    int rank = square / BOARD_LENGTH;
    int file = square % BOARD_LENGTH;
    return (rank + file) % 2 == 0;
}

void Board::load_fen(const std::string& fen)
{
    reset_board();
    fen_parser(fen);
    init_pieces_per_side_bitboard();
    init_piece_list();
    game_state.zobrist_key = init_zobrist_key();
}


/*=========================
    GAME STATE STRUCT
=========================*/

GameState::GameState() 
{ 
    move = Move(); 
} 


void GameState::clear()
{
    active_color = WHITE;
    castling = 0;
    half_move_clock = 0;
    en_passant = ep_none;          
    fullmove_number = 0;
    zobrist_key = 0;
}

U8 GameState::enpassant_to_square(U8 enpassant_num)
{
    if (enpassant_num < 0 || enpassant_num >= ep_none) 
    {
        return ep_none; // invalid input
    }
    
    if (enpassant_num <= ep_white_h) 
    {
        return a2 + enpassant_num;
    }
    
    return a6 + (enpassant_num - BOARD_LENGTH);
}

void GameState::print_game_state() 
{
    std::cout << "GameState: [Color:" << (active_color == WHITE ? "WHITE" : "BLACK")
              << " | Castling:" << (int)castling 
              << " | Half:" << (int)half_move_clock
              << " | EP:" << (int)en_passant
              << " | Full:" << fullmove_number
              << " | Zobrist:0x" << std::hex << zobrist_key << std::dec
              << "]\n";
}

/*=========================
    HISTORY STRUCT
=========================*/

History::History()
{  }

void History::push(GameState game_state)
{
    list.at(count) = game_state;
    count++;
}

void History::pop() 
{
    count -= 1;
}

void History::clear()
{
    count = 0;
}
 
U16 History::len() //NOLINT
{
    return count;
}

GameState& History::get_ref(size_t index)
{
    return list.at(index);
}
