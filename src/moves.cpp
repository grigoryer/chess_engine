#include <moves.hpp>





//constructors for move
Move::Move(Square from, Square to, Piece piece) : data(from | (to << TO_SHIFT) | (piece << PIECE_SHIFT))
{ }

Move::Move(Square from, Square to, Piece piece, bool capture) : data(from | (to << TO_SHIFT) | (piece << PIECE_SHIFT) | (capture << CAPTURE_SHIFT))
{ }

Move::Move(Square from, Square to, Piece piece, bool capture, Piece promotion) 
: data(from | (to << TO_SHIFT) | (piece << PIECE_SHIFT) | (capture << CAPTURE_SHIFT) | (promotion << PROMOTED_SHIFT))
{ }

Move::Move(Square from, Square to, Piece piece, bool capture, Piece promotion, bool doublePush, bool enpassant, bool castle)
: data(from | (to << TO_SHIFT) | (capture << CAPTURE_SHIFT) | (piece << PIECE_SHIFT) | (promotion << PROMOTED_SHIFT)
    | (doublePush << DOUBLE_SHIFT) | (enpassant << ENPASSANT_SHIFT) | (castle << CASTLE_SHIFT))
{ }