#pragma once
#include "constants.hpp"
#include <position.hpp>

//debug functions
void printBoard(Bitboard bitboard);

void printPieceBoard(Board &b);

void printState(State& state, Board& b);

void printDebug(Board& b);

int perft(Board& b, int depth);

U64 perftDivide(Board& b, int depth);

int perftDebug(Board& b, int depth);

void perftDivideDebug(Board& b, int depth);