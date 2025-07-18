#include "bitboards.hpp"
#include <sstream>
#include <cstring>  

/*=========================
    DEBUG FUNCTIONS
=========================*/

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

void Board::print_piece_list() 
{

    const char piece_to_char[13] = 
    {
        [NONE] = '.',
        [K] = 'K',
        [Q] = 'Q', 
        [R] = 'R',
        [B] = 'B',
        [N] = 'N',
        [P] = 'P',
        [k] = 'k',
        [q] = 'q',
        [r] = 'r',
        [b] = 'b',
        [n] = 'n',
        [p] = 'p'
    };

    std::cout << "\nPiece List Array:\n";

    for (int rank = 7; rank >= 0; rank--) 
    {  
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++) 
        {
            int square = rank * 8 + file;
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
    std::vector<std::string> parts;
    std::istringstream iss(fen);
    std::string word;

    while(iss >> word) {
        parts.push_back(word);
    }

    //split up string into parts
    int rank = 0; 
    int file = 0;
    std::string position = parts[0];
    std::string color = parts[1];
    std::string castling = parts[2];
    std::string en_passant = parts[3];
    std::string half_move = parts[4];
    std::string move = parts[5];

    //handle the first position part of the fen string and add bits where specified.
    for(int i = 0; i < position.length(); i++)
    {
        char ch = position[i];

        if(ch == '/')
        {
            rank++;
            file = 0;
        }
        else if (std::isdigit(ch)) 
        {
            file += (ch - '0');
        }
        else
        {
            bool isWhite = !isupper(ch);
            int square = (7 - rank) * 8 + file;
            ch = tolower(ch);

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

    //create hte castling int
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
        if(rank == 3) 
        {
            game_state.en_passant = file; // 0-7
        }
        else if(rank == 6) 
        {
            game_state.en_passant = 8 + file; 
        }
    }
    else
    {
        game_state.en_passant = 16; //none value
    }

    if(half_move == "-" || parts.size() < 5){game_state.half_move_clock = 0;}
    else {game_state.half_move_clock = std::stoi(half_move);}

    if(move == "-" || parts.size() < 5){game_state.fullmove_number = 0;}
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
            piece_list[square] = (PieceList)(K + piece_type);
            pop_bit(bb_w, square);
        }
        
        U64 bb_b = bb_pieces[BLACK][piece_type];
        while(bb_b)
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
    memset(bb_pieces, 0, sizeof(bb_pieces));
    memset(bb_side, 0, sizeof(bb_side));
    memset(piece_list, NONE, sizeof(piece_list));

    game_state.clear();
    history.clear();
}

int Board::king_sqaure(int side)
{
    return lsb(bb_pieces[side][KING]);
}

U64& Board::get_piece(int side, int Piece)
{
    return bb_pieces[side][Piece];
}

U64& Board::get_side(int side)
{
    return bb_side[side];
}

bool Board::has_bishop_pair(int side) 
{
    U64 bb_bishop = get_piece(side, BISHOP);

    if(bits_in_bitboard(bb_bishop) < 2) return false;

    int count_dark = 0;
    int count_light = 0;

    while(bb_bishop)
    {
        int square = lsb(bb_bishop);
        if(is_white_square(square)) count_light++;
        else count_dark++;
        pop_bit(bb_bishop, square);
    }

    return (count_dark > 0 && count_light > 0);
}

bool Board::is_white_square(int square)
{
    int rank = square / 8;
    int file = square % 8;
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
    : active_color(WHITE)
    , castling(0)
    , half_move_clock(0)
    , en_passant(16)
    , fullmove_number(1)
    , zobrist_key(0)
{ move = Move(); } 


void GameState::clear()
{
    active_color = WHITE;
    castling = 0;
    half_move_clock = 0;
    en_passant = 16;          
    fullmove_number = 1;
    zobrist_key = 0;
}

int GameState::enpassant_to_square(int enpassant_num)
{
    if (enpassant_num < 0 || enpassant_num > 15) {
        return -1; // Invalid input
    }
    
    if (enpassant_num <= 7) {
        // Files a-h on rank 3 (squares 16-23)
        return 16 + enpassant_num;
    } else {
        // Files a-h on rank 6 (squares 40-47)
        return 40 + (enpassant_num - 8);
    }
}

/*=========================
    HISTORY STRUCT
=========================*/

History::History()
    : count(0)
{  }

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
