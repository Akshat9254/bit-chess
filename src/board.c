#include <assert.h>
#include <string.h>

#include "board.h"
#include "piece.h"

void clear_board(Board *board) {
    memset(board, 0, sizeof(*board));
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        board->mailbox[sq] = PIECE_NONE;
    }

    board->side_to_move = COLOR_WHITE;
    board->enpassant_sq = SQ_NONE;
    board->castling_rights = CASTLE_ANY;
    board->full_move_number = 1;
}

void place_piece_on_sq(Board *board, const Piece piece, const Square sq) {
    assert(piece != PIECE_NONE);
    assert(sq != SQ_NONE);
    assert(is_sq_empty(board, sq));

    const Color piece_color = piece_color_of(piece);

    bb_set(&board->pieces[piece], sq);
    bb_set(&board->occupied[piece_color], sq);
    bb_set(&board->occupied[COLOR_BOTH], sq);
    board->mailbox[sq] = piece;
}

void clear_sq(Board *board, const Square sq) {
    assert(sq != SQ_NONE);
    assert(!is_sq_empty(board, sq));

    const Piece piece = board->mailbox[sq];
    const Color piece_color = piece_color_of(piece);

    bb_clear(&board->pieces[piece], sq);
    bb_clear(&board->occupied[piece_color], sq);
    bb_clear(&board->occupied[COLOR_BOTH], sq);
    board->mailbox[sq] = PIECE_NONE;
}
