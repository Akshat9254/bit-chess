#include <stdio.h>
#include <stdlib.h>

#include "move.h"
#include "square.h"

static inline U8 promo_piece_index(char symbol);

void move_to_string(const Move move, const Color color, char *str, const size_t size) {
    const Square from = move_get_from(move);
    const Square to = move_get_to(move);
    const U8 flags = move_get_flags(move);

    const char from_file = (char) ('a' + file_of_sq(move_get_from(move)));
    const char from_rank = (char) ('1' + rank_of_sq(from));
    const char to_file = (char) ('a' + file_of_sq(to));
    const char to_rank = (char) ('1' + rank_of_sq(to));

    if (is_promo(flags)) {
        const char promotion = piece_symbol_of(move_get_promo_piece(flags, color));
        snprintf(str, size, "%c%c%c%c%c", from_file, from_rank, to_file, to_rank, promotion);
    } else {
        snprintf(str, size, "%c%c%c%c", from_file, from_rank, to_file, to_rank);
    }
}

Move move_from_string(const char *str, const Board *board) {
    assert(strlen(str) >= 4);

    const Square from = string_to_sq(str);
    const Square to = string_to_sq(str + 2);
    const PieceType from_piece_type = piece_type_of(board->mailbox[from]);

    U8 flags = MOVE_QUIET;

    if (from_piece_type == PT_PAWN) {
        if (to == board->enpassant_sq) {
            flags = MOVE_ENPASSANT;
        } else if (abs(rank_of_sq(to) - rank_of_sq(from)) == 2) {
            flags = MOVE_PAWN_DOUBLE_PUSH;
        }
    }

    if (from_piece_type == PT_KING) {
        if (from == SQ_E1) {
            if (to == SQ_G1) flags = MOVE_KING_CASTLE;
            if (to == SQ_C1) flags = MOVE_QUEEN_CASTLE;
        } else if (from == SQ_E8) {
            if (to == SQ_G8) flags = MOVE_KING_CASTLE;
            if (to == SQ_C8) flags = MOVE_QUEEN_CASTLE;
        }
    }

    if (str[4] == 'n' || str[4] == 'b' || str[4] == 'r' || str[4] == 'q') {
        flags = MOVE_MASK_PROMO | promo_piece_index(str[4]);
    }

    if (board->mailbox[to] != PIECE_NONE) {
        flags |= MOVE_MASK_CAP;
    }

    return create_move(from, to, flags);
}

static inline U8 promo_piece_index(const char symbol) {
    switch (symbol) {
        case 'n': return 0;
        case 'b': return 1;
        case 'r': return 2;
        case 'q': return 3;
        default: return -1;
    }
}
