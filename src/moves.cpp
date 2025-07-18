#include "moves.hpp"
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

Move::Move() : move(0) { }

Move::Move(int source, int target, int piece, int promoted, 
           bool capture, bool double_push, bool enpassant, bool castling) 
           {
            move = source | 
           (target << 6) | 
           (piece << 12) | 
           (promoted << 16) | 
           (capture << 20) | 
           (double_push << 21) | 
           (enpassant << 22) | 
           (castling << 23);
}

int Move::get_source() const 
{ 
    return move & SOURCE_MASK; 
}

int Move::get_target() const 
{ 
    return (move & TARGET_MASK) >> 6; 
}

int Move::get_piece() const 
{ 
    return (move & PIECE_MASK) >> 12; 
}

int Move::get_promoted() const 
{ 
    return (move & PROMOTED_MASK) >> 16; 
}

bool Move::is_capture() const 
{ 
    return move & CAPTURE_MASK; 
}

bool Move::is_double() const 
{ 
    return move & DOUBLE_MASK; 
}

bool Move::is_enpassant() const 
{ 
    return move & ENPASSANT_MASK; 
}

bool Move::is_castling() const 
{ 
    return move & CASTLING_MASK; 
}

// Setters
void Move::set_source(int source) 
{ 
    move = (move & ~SOURCE_MASK) | source; 
}

void Move::set_target(int target) 
{ 
    move = (move & ~TARGET_MASK) | (target << 6); 
}

void Move::set_piece(int piece) 
{ 
    move = (move & ~PIECE_MASK) | (piece << 12); 
}

void Move::set_promoted(int promoted) 
{ 
    move = (move & ~PROMOTED_MASK) | (promoted << 16); 
}

void Move::set_capture(bool capture) 
{ 
    move = (move & ~CAPTURE_MASK) | (capture << 20); 
}

void Move::set_double(bool double_push) 
{ 
    move = (move & ~DOUBLE_MASK) | (double_push << 21); 
}

void Move::set_enpassant(bool enpassant) 
{ 
    move = (move & ~ENPASSANT_MASK) | (enpassant << 22); 
}

void Move::set_castling(bool castling) 
{ 
    move = (move & ~CASTLING_MASK) | (castling << 23); 
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

bool Move::is_empty() const {
    return move == 0;
}

bool Move::is_promotion() const {
    return get_promoted() != 0;
}

bool Move::is_quiet() const {
    return !is_capture() && !is_double() && !is_enpassant() && !is_castling() && !is_promotion();
}

void Move::print_move(U8 active_color) const {
    std::cout << "\n" << n_to_piece(get_piece(),active_color);

    if(is_capture()){
        std::cout << "x";
    }

    std::cout << n_to_sq(get_target()) << " ";
}

std::string Move::n_to_sq(U32 num) const
{
    // Calculate file (column) and rank (row)
    int file = num % 8;  // 0-7 maps to a-h
    int rank = num / 8;  // 0-7 maps to 1-8
    
    char file_letter = 'a' + file;
    char rank_number = '1' + rank;
    
    return std::string(1, file_letter) + rank_number;
}

std::string Move::n_to_piece(U32 num, U8 active_color) const
{
    !active_color ? num : num += 6;
    std::string pieces[] = {"K", "Q", "R", "B", "N", "", 
                            "k", "q", "r", "b", "n", ""};
    return pieces[num];
}

void MoveList::add(const Move& move)
{
    moves[count++] = move;
}

void MoveList::clear()
{
    memset(moves, 0, 256);
    count = 0;
}

bool MoveList::is_move_empty(int i) const {
    if (i < 0 || i >= count) {
        return true; 
    }
    return moves[i].get_raw() == 0;
}
