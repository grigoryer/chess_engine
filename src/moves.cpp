#include "moves.hpp"
#include "types.hpp"
#include <cassert>
#include <string>
#include <iostream>

/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

static const U8 target_shift = 6;
static const U8 piece_shift = 12;
static const U8 promoted_shift = 16;
static const U8 capture_shift = 20;
static const U8 double_shift = 21;
static const U8 enpassant_shift = 22;
static const U8 castling_shift = 23;


Move::Move() : move(0) { }

Move::Move(U8 source, U8 target, U8 piece, U8 promoted, 
           bool capture, bool double_push, bool enpassant, bool castling)
    : move (
        static_cast<U32>(source)
        | (static_cast<U32>(target)      << target_shift)
        | (static_cast<U32>(piece)       << piece_shift)
        | (static_cast<U32>(promoted)    << promoted_shift)
        | (static_cast<U32>(capture)     << capture_shift)
        | (static_cast<U32>(double_push) << double_shift)
        | (static_cast<U32>(enpassant)   << enpassant_shift)
        | (static_cast<U32>(castling)    << castling_shift)
      )
{  }


U8 Move::get_source() const 
{ 
    return move & SOURCE_MASK; 
}

U8 Move::get_target() const 
{ 
    return (move & TARGET_MASK) >> target_shift; 
}

U8 Move::get_piece() const 
{ 
    return (move & PIECE_MASK) >> piece_shift; 
}

U8 Move::get_promoted() const 
{ 
    return (move & PROMOTED_MASK) >> promoted_shift; 
}

bool Move::is_capture() const 
{ 
    return (move & CAPTURE_MASK) != 0; 
}

bool Move::is_double() const 
{ 
    return (move & DOUBLE_MASK) != 0; 
}

bool Move::is_enpassant() const 
{ 
    return (move & ENPASSANT_MASK) != 0; 
}

bool Move::is_castling() const 
{ 
    return (move & CASTLING_MASK) != 0; 
}

// setters
void Move::set_source(U8 source) 
{ 
    move = (move & ~SOURCE_MASK) | source; 
}

void Move::set_target(U8 target) 
{ 
    move = (move & ~TARGET_MASK) | (target << target_shift); 
}

void Move::set_piece(U8 piece) 
{ 
    move = (move & ~PIECE_MASK) | (piece << piece_shift); 
}

void Move::set_promoted(U8 promoted) 
{ 
    move = (move & ~PROMOTED_MASK) | (promoted << promoted_shift); 
}
void Move::set_capture(bool capture) 
{ 
    move = (move & ~CAPTURE_MASK) | (static_cast<U32>(capture) << capture_shift); 
}

void Move::set_double(bool double_push) 
{ 
    move = (move & ~DOUBLE_MASK) | (static_cast<U32>(double_push) << double_shift); 
}

void Move::set_enpassant(bool enpassant) 
{ 
    move = (move & ~ENPASSANT_MASK) | (static_cast<U32>(enpassant) << enpassant_shift); 
}

void Move::set_castling(bool castling) 
{ 
    move = (move & ~CASTLING_MASK) | (static_cast<U32>(castling) << castling_shift); 
}

bool Move::is_null() const 
{ 
    return move == 0; 
}

void Move::clear() 
{ 
    move = 0; 
}

U32 Move::get_raw() const 
{ 
    return move; 
}

bool Move::is_empty() const 
{
    return move == 0;
}

bool Move::is_promotion() const 
{
    return get_promoted() != 0;
}

bool Move::is_quiet() const 
{
    return !is_capture() && !is_double() && !is_enpassant() && !is_castling() && !is_promotion();
}

bool Move::is_special() const 
{
    return is_enpassant() || is_castling() || is_promotion();
}

void Move::print_move(U8 active_color) const 
{
    std::cout << "\nMove: " << n_to_piece(get_piece(), active_color);

    if(is_capture()){
        std::cout << "x";
    }

    std::cout << n_to_sq(get_target());

    if(is_promotion()){
        std::cout << "=";
        switch(get_promoted())
        {
            case(n_promotion): std::cout << (active_color == WHITE ? "N" : "n"); break;
            case(b_promotion): std::cout << (active_color == WHITE ? "B" : "b"); break;
            case(r_promotion): std::cout << (active_color == WHITE ? "R" : "r"); break;
            case(q_promotion): std::cout << (active_color == WHITE ? "Q" : "q"); break; //NOLINT
            default: std::cout << (active_color == WHITE ? "Q" : "q"); break; // fallback
        }
    }

    if(is_enpassant()){
        std::cout << " e.p.";
    }
    std::cout << "\n";
}

std::string Move::n_to_sq(U8 num)
{
    // calculate file (column) and rank (row)
    U8 file = num % BOARD_LENGTH;  // 0-7 maps to a-h
    U8 rank = num / BOARD_LENGTH;  // 0-7 maps to 1-8
    
    char file_letter = static_cast<char>('a' + file);
    char rank_number = static_cast<char>('1' + rank);
    
    return std::string(1, file_letter) + rank_number;
}

std::string Move::n_to_piece(U8 piece_type, U8 active_color) {

    piece_type +=  ((active_color == WHITE) ? 0 : NUM_PIECES);

    std::array<std::string, UNIQUE_PIECES> pieces = {
        "K", "Q", "R", "B", "N", "",  // 0-5: white pieces
        "k", "q", "r", "b", "n", ""   // 6-11: black pieces
    };
    
    if (piece_type >= UNIQUE_PIECES) {
        return "?"; // Invalid piece type
    }
    
    return pieces[piece_type];
}

void MoveList::add(const Move move) {
    assert(count < 256 && "MoveList overflow");
    moves.at(count++) = move;
}

void MoveList::clear()
{
    count = 0;
}

bool MoveList::is_move_empty(U16 index) const 
{
    if (index < 0 || index >= count) {
        return true; 
    }
    return moves.at(index).get_raw() == 0;
}