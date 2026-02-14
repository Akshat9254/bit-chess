#ifndef BOARD_H
#define BOARD_H

#include "defs.h"
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
bool is_valid_sq(Square sq);

/* helpers */
uint8_t rank_of_sq(Square sq);
char file_of_sq(Square sq);
uint8_t rank_dist(Square sq1, Square sq2);
uint8_t file_dist(Square sq1, Square sq2);
Square to_sq(uint8_t rank, char file);
char piece_symbol_of(Piece piece);
Color piece_color_of(Piece piece);
Piece piece_on_sq(const Board *b, Square sq);
Bitboard board_occupancy(const Board *board);
Bitboard enemy_board_occupancy(const Board *board);
Bitboard current_side_occupancy(const Board *board);

#endif
