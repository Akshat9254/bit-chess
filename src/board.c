#include <string.h>
#include <stdio.h>
#include "board.h"

const char* piece_symbols = "PNBRQKpnbrqk__";

void init_board(Board *board) {
	clear_board(board);
	
	/* pawns */
	board->pieces[WHITE_PAWN] = RANK_2;
	board->occupied[WHITE] |= board->pieces[WHITE_PAWN];

	board->pieces[BLACK_PAWN] = RANK_7;
	board->occupied[BLACK] |= board->pieces[BLACK_PAWN];
	
	/* knights */
	place_piece_on_sq(board, WHITE_KNIGHT, B1);
	place_piece_on_sq(board, WHITE_KNIGHT, G1);
	
	place_piece_on_sq(board, BLACK_KNIGHT, B8);
	place_piece_on_sq(board, BLACK_KNIGHT, G8);
	
	/* bishop */
	place_piece_on_sq(board, WHITE_BISHOP, C1);
	place_piece_on_sq(board, WHITE_BISHOP, F1);

	place_piece_on_sq(board, BLACK_BISHOP, C8);
	place_piece_on_sq(board, BLACK_BISHOP, F8);

	/* rooks */
	place_piece_on_sq(board, WHITE_ROOK, A1);
	place_piece_on_sq(board, WHITE_ROOK, H1);

	place_piece_on_sq(board, BLACK_ROOK, A8);
	place_piece_on_sq(board, BLACK_ROOK, H8);

	/* queens */
	place_piece_on_sq(board, WHITE_QUEEN, D1);
	place_piece_on_sq(board, BLACK_QUEEN, D8);
	
	/* kings */
	place_piece_on_sq(board, WHITE_KING, E1);
	place_piece_on_sq(board, BLACK_KING, E8);

	board->side_to_move = WHITE;
}

inline void clear_board(Board *board) {
    memset(board, 0, sizeof(*board));
}

void place_piece_on_sq(Board *board, Piece piece, Square sq) {
	if (!is_valid_sq(sq)) {
		return;
	}

	Color piece_color = piece_color_of(piece);
	if (bb_test(board_occupancy(board), sq)) {
		return;
	}

	bb_set(&board->pieces[piece], sq);
	bb_set(&board->occupied[piece_color], sq);
}

void clear_sq(Board *board, Square sq) {
	Piece piece = piece_on_sq(board, sq);

	if (piece == NO_PIECE) {
		return;
	}

	Color piece_color = piece_color_of(piece);
	bb_clear(&board->pieces[piece], sq);
	bb_clear(&board->occupied[piece_color], sq);
}

inline bool is_valid_sq(Square sq) {
	return sq >= 0 && sq < SQ_NB;
}

inline uint8_t rank_of_sq(Square sq) {
	return 1 + (sq / FILE_NB);
}

inline char file_of_sq(Square sq) {
	return 'A' + (sq % FILE_NB);
}

inline Square to_sq(uint8_t rank, char file) {
	Square sq = (rank - 1) * FILE_NB + (file - 'A');
	return is_valid_sq(sq) ? sq : NO_SQ;
}

inline char piece_symbol_of(Piece piece) {
	return piece_symbols[piece];
}

inline Color piece_color_of(Piece piece) {
	return piece <= WHITE_KING ? WHITE : BLACK;
}

Piece piece_on_sq(const Board *board, Square sq) {
	Bitboard all = board_occupancy(board);
	if (!bb_test(all, sq)) {
		return NO_PIECE;
	}

	for (Piece piece = 0; piece < PIECE_NB; piece++) {
		if (bb_test(board->pieces[piece], sq)) {
			return piece;
		}
	}

	fprintf(stderr, "square: %d is not empty and no piece is on it\n", sq);
	return NO_PIECE;
}

inline Bitboard board_occupancy(const Board *board) {
	return board->occupied[WHITE] | board->occupied[BLACK];
}

inline Bitboard enemy_board_occupancy(const Board *board) {
	Color enemy_color = board->side_to_move ^ 1;
	return board->occupied[enemy_color];
}

inline Bitboard current_side_occupancy(const Board *board) {
	return board->occupied[board->side_to_move];
}
