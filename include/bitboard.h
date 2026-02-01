#ifndef BITBOARD_H
#define BITBOARD_H

#include "defs.h"

#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL

typedef struct {
	Bitboard pieces[COLOR_NB][PIECE_NB];
	Bitboard occupied[COLOR_NB];
	Bitboard all;
} Board;

void init_board(Board *b);
void bb_print(Bitboard bb);

#endif
