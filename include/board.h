#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"

typedef struct {
    Bitboard pieces[PIECE_NB];
    Bitboard occupied[COLOR_NB];
    Piece mailbox[SQ_NB];
    Square kings_sq[COLOR_BOTH];

    Color side_to_move;
    Square enpassant_sq;
    CastlingRights castling_rights;

    U8 half_move_clock;
    U16 full_move_number;
} Board;

void clear_board(Board *board);
void place_piece_on_sq(Board *board, Piece piece, Square sq);
void clear_sq(Board *board, Square sq);
bool is_sq_attacked(const Board *board, Square sq, Color attacker);
void print_board(const Board* board);

static inline bool is_king_in_check(const Board *board, const Color color) {
    return is_sq_attacked(board, board->kings_sq[color], color ^ 1);
}

static inline bool is_sq_empty(const Board *board, const Square sq) {
    return board->mailbox[sq] == PIECE_NONE;
}

#endif
