#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include "chess_types.h"
#include "bitboard.h"

typedef struct {
	Bitboard pieces[PIECE_NB];
	Bitboard occupied[COLOR_NB];
	Color side_to_move;
	Square enpassant_sq;
} Board;

void init_board(Board *b);
void clear_board(Board *b);
void place_piece_on_sq(Board *board, Piece piece, Square sq);
void clear_sq(Board *board, Square sq);
bool is_sq_attacked(const Board *board, const Square sq, const Color attacker);
Piece piece_on_sq(const Board *b, Square sq);

Bitboard board_occupancy(const Board *board);
Bitboard enemy_board_occupancy(const Board *board);
Bitboard current_side_occupancy(const Board *board);

#endif
