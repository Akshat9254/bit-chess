#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdbool.h>
#include "defs.h"

#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL

extern const char* piece_symbols;

typedef struct {
	Bitboard pieces[COLOR_NB][PIECE_NB];
	Bitboard occupied[COLOR_NB];
	Bitboard all;
} Board;

void init_board(Board *b);
char get_piece_symbol_on_sq(Board *b, int sq);
void bb_print(Bitboard bb);
bool bb_test(Bitboard bb, int sq);

#endif
