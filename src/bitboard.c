#include <string.h>
#include <stdio.h>
#include "bitboard.h"

const char* piece_symbols = "PNBRQKpnbrqk";

void init_board(Board *b) {
	memset(b, 0, sizeof(*b));
	
	/* pawns */
	b->pieces[WHITE][PAWN] = RANK_2;
	b->occupied[WHITE] |= b->pieces[WHITE][PAWN];

	b->pieces[BLACK][PAWN] = RANK_7;
	b->occupied[BLACK] |= b->pieces[BLACK][PAWN];
	
	/* knights */
	b->pieces[WHITE][KNIGHT] = ((1ULL << B1) | (1ULL << G1));
	b->occupied[WHITE] |= b->pieces[WHITE][KNIGHT];
	
	b->pieces[BLACK][KNIGHT] = ((1ULL << B8) | (1ULL << G8));
	b->occupied[BLACK] |= b->pieces[BLACK][KNIGHT];
	
	/* bishop */
	b->pieces[WHITE][BISHOP] = ((1ULL << C1) | (1ULL << F1));
	b->occupied[WHITE] |= b->pieces[WHITE][BISHOP];
	
	b->pieces[BLACK][BISHOP] = ((1ULL << C8) | (1ULL << F8));
	b->occupied[BLACK] |= b->pieces[BLACK][BISHOP];

	/* rooks */
	b->pieces[WHITE][ROOK] = ((1ULL << A1) | (1ULL << H1));
	b->occupied[WHITE] |= b->pieces[WHITE][ROOK];

	b->pieces[BLACK][ROOK] = ((1ULL << A8) | (1ULL << H8));
	b->occupied[BLACK] |= b->pieces[BLACK][ROOK];

	/* queens */
	b->pieces[WHITE][QUEEN] = (1ULL << D1);
	b->occupied[WHITE] |= b->pieces[WHITE][QUEEN];

	b->pieces[BLACK][QUEEN] = (1ULL << D8);
	b->occupied[BLACK] |= b->pieces[BLACK][QUEEN];
	
	/* kings */
	b->pieces[WHITE][KING] = (1ULL << E1);
	b->occupied[WHITE] |= b->pieces[WHITE][KING];
	
	b->pieces[BLACK][KING] = (1ULL << E8);
	b->occupied[BLACK] |= b->pieces[BLACK][KING];
	
	b->all = (b->occupied[WHITE] | b->occupied[BLACK]);
}

char get_piece_symbol_on_sq(Board *b, int sq) {
	Bitboard all = b->occupied[WHITE] | b->occupied[BLACK];
	if (bb_test(all, sq) == 0) {
		return '_';
	}

	for (int color = 0; color < COLOR_NB; color++) {
		for (int piece = 0; piece < PIECE_NB; piece++) {
			if (bb_test(b->pieces[color][piece], sq) == 1) {
				return piece_symbols[PIECE_NB * color + piece];
			}
		}
	}

	fprintf(stderr, "square: %d is not empty and no piece is on it\n", sq);
	return '\0';
}

void bb_print(Bitboard bb) {	
	for (int rank = RANK_NB - 1; rank >= 0; rank--) {
		for (int file = 0; file < FILE_NB; file++) {
			int sq = rank * FILE_NB + file;
			int is_occupied = (bb & (1ULL << sq)) > 0;
			printf("%c\t", (is_occupied ? '1': '0'));
		}
		
		printf("\n");
	}
}

bool bb_test(Bitboard bb, int sq) {
	return (bb & (1ULL << sq)) > 0;
}
