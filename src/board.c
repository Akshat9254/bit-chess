#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "square.h"
#include "board.h"
#include "attacks.h"

static bool pawn_attack_sq(const Board *board, const Square sq, const Color attacker);
static bool knight_attack_sq(const Board *board, const Square sq, const Color attacker);

void init_board(Board *board) {
	clear_board(board);
	
	/* pawns */
	board->pieces[WHITE_PAWN] = RANK_2_MASK;
	board->occupied[WHITE] |= board->pieces[WHITE_PAWN];

	board->pieces[BLACK_PAWN] = RANK_7_MASK;
	board->occupied[BLACK] |= board->pieces[BLACK_PAWN];
	
	/* knights */
	place_piece_on_sq(board, WHITE_KNIGHT, b1);
	place_piece_on_sq(board, WHITE_KNIGHT, g1);
	
	place_piece_on_sq(board, BLACK_KNIGHT, b8);
	place_piece_on_sq(board, BLACK_KNIGHT, g8);
	
	/* bishop */
	place_piece_on_sq(board, WHITE_BISHOP, c1);
	place_piece_on_sq(board, WHITE_BISHOP, f1);

	place_piece_on_sq(board, BLACK_BISHOP, c8);
	place_piece_on_sq(board, BLACK_BISHOP, f8);

	/* rooks */
	place_piece_on_sq(board, WHITE_ROOK, a1);
	place_piece_on_sq(board, WHITE_ROOK, h1);

	place_piece_on_sq(board, BLACK_ROOK, a8);
	place_piece_on_sq(board, BLACK_ROOK, h8);

	/* queens */
	place_piece_on_sq(board, WHITE_QUEEN, d1);
	place_piece_on_sq(board, BLACK_QUEEN, d8);
	
	/* kings */
	place_piece_on_sq(board, WHITE_KING, e1);
	place_piece_on_sq(board, BLACK_KING, e8);

	board->side_to_move = WHITE;
	board->enpassant_sq = NO_SQ;
}

inline void clear_board(Board *board) {
    memset(board, 0, sizeof(*board));
	board->enpassant_sq = NO_SQ;
}

void place_piece_on_sq(Board *board, Piece piece, Square sq) {
	if (!is_sq_on_board(sq)) {
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

bool is_sq_attacked(const Board *board, const Square sq, const Color attacker) {
	return false;
}

static bool pawn_attack_sq(const Board *board, const Square sq, const Color attacker) {
	return (pawn_attacks[!attacker][sq] & board->occupied[attacker == WHITE ? WHITE_PAWN : BLACK_PAWN]);
}

static bool knight_attack_sq(const Board *board, const Square sq, const Color attacker) {

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
