#include "constants.hpp"
#include "moves.hpp"
#include <move_generation.hpp>
#include <attacks.hpp>



ExtdMove* generateLegals(ExtdMove* list, Board& b)
{
    list = generateMoves<QUIET>(list, b);
    list = generateMoves<CAPTURE>(list, b);
}