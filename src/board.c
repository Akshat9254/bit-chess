#include <string.h>
#include <stdio.h>
#include "board.h"

const char* piece_symbols = "PNBRQKpnbrqk__";

void init_board(Board *b) {
	memset(b, 0, sizeof(*b));
	
	/* pawns */
	b->pieces[WHITE_PAWN] = RANK_2;
	b->occupied[WHITE] |= b->pieces[WHITE_PAWN];

	b->pieces[BLACK_PAWN] = RANK_7;
	b->occupied[BLACK] |= b->pieces[BLACK_PAWN];
	
	/* knights */
	b->pieces[WHITE_KNIGHT] = ((1ULL << B1) | (1ULL << G1));
	b->occupied[WHITE] |= b->pieces[WHITE_KNIGHT];
	
	b->pieces[BLACK_KNIGHT] = ((1ULL << B8) | (1ULL << G8));
	b->occupied[BLACK] |= b->pieces[BLACK_KNIGHT];
	
	/* bishop */
	b->pieces[WHITE_BISHOP] = ((1ULL << C1) | (1ULL << F1));
	b->occupied[WHITE] |= b->pieces[WHITE_BISHOP];
	
	b->pieces[BLACK_BISHOP] = ((1ULL << C8) | (1ULL << F8));
	b->occupied[BLACK] |= b->pieces[BLACK_BISHOP];

	/* rooks */
	b->pieces[WHITE_ROOK] = ((1ULL << A1) | (1ULL << H1));
	b->occupied[WHITE] |= b->pieces[WHITE_ROOK];

	b->pieces[BLACK_ROOK] = ((1ULL << A8) | (1ULL << H8));
	b->occupied[BLACK] |= b->pieces[BLACK_ROOK];

	/* queens */
	b->pieces[WHITE_QUEEN] = (1ULL << D1);
	b->occupied[WHITE] |= b->pieces[WHITE_QUEEN];

	b->pieces[BLACK_QUEEN] = (1ULL << D8);
	b->occupied[BLACK] |= b->pieces[BLACK_QUEEN];
	
	/* kings */
	b->pieces[WHITE_KING] = (1ULL << E1);
	b->occupied[WHITE] |= b->pieces[WHITE_KING];
	
	b->pieces[BLACK_KING] = (1ULL << E8);
	b->occupied[BLACK] |= b->pieces[BLACK_KING];

	b->side_to_move = WHITE;
}

void clear_board(Board *b) {
    for (Piece piece = 0; piece < PIECE_NB; piece++) {
        b->pieces[piece] = 0;
    }

    b->occupied[WHITE] = b->occupied[BLACK] = 0;
    b->side_to_move = WHITE;
}

void place_piece_on_sq(Board *board, Piece piece, Square sq) {
	if (sq < 0 || sq >= SQ_NB) {
		return;
	}

	Color piece_color = piece_color_of(piece);
	if (bb_test(board->occupied[WHITE] | board->occupied[BLACK], sq)) {
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

inline char piece_symbol_of(Piece piece) {
	return piece_symbols[piece];
}

inline Color piece_color_of(Piece piece) {
	return piece <= WHITE_KING ? WHITE : BLACK;
}

Piece piece_on_sq(const Board *b, Square sq) {
	Bitboard all = b->occupied[WHITE] | b->occupied[BLACK];
	if (!bb_test(all, sq)) {
		return NO_PIECE;
	}

	for (Piece piece = 0; piece < PIECE_NB; piece++) {
		if (bb_test(b->pieces[piece], sq)) {
			return piece;
		}
	}

	fprintf(stderr, "square: %d is not empty and no piece is on it\n", sq);
	return NO_PIECE;
}
