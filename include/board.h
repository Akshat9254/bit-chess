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

    ZobristHash hash;
} Board;

typedef struct StateInfo {
    struct StateInfo *previous_state;
    Move move;
    Square en_passant_sq;
    CastlingRights castling_rights;
    Piece captured_piece;
    U8 half_move_clock;
} StateInfo;

void clear_board(Board *board);
void reset_board(Board *board);
void place_piece_on_sq(Board *board, Piece piece, Square sq);
void clear_sq(Board *board, Square sq);
void make_move(Board *board, Move move, StateInfo *state);
void unmake_move(Board *board, const StateInfo *state);
bool is_sq_attacked(const Board *board, Square sq, Color attacker);
void print_board(const Board* board);

static inline void move_piece(Board *board, const Piece piece, const Square from, const Square to) {
    clear_sq(board, from);
    place_piece_on_sq(board, piece, to);
}

static inline bool is_king_in_check(const Board *board, const Color color) {
    return is_sq_attacked(board, board->kings_sq[color], color ^ 1);
}

static inline bool is_king_alive(const Board *board, const Color color) {
    return board->pieces[WHITE_KING + color * PIECE_PER_SIDE] > 0;
}

static inline bool is_sq_empty(const Board *board, const Square sq) {
    return board->mailbox[sq] == PIECE_NONE;
}

#endif
