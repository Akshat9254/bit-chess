#include <assert.h>
#include <string.h>

#include "board.h"
#include "fen_parser.h"
#include "leaper_attack_tables.h"
#include "magic_attack_tables.h"
#include "piece.h"

static const bool is_king_piece[PIECE_NB] = {
    [WHITE_KING] = true, [BLACK_KING] = true
};

void clear_board(Board *board) {
    memset(board, 0, sizeof(*board));
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        board->mailbox[sq] = PIECE_NONE;
    }

    board->side_to_move = COLOR_WHITE;
    board->enpassant_sq = SQ_NONE;
    board->castling_rights = CASTLE_ANY;
    board->full_move_number = 1;
    board->kings_sq[COLOR_WHITE] = board->kings_sq[COLOR_BLACK] = SQ_NONE;
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

    if (is_king_piece[piece]) {
        board->kings_sq[piece_color] = sq;
    }
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

    if (is_king_piece[piece]) {
        board->kings_sq[piece_color] = SQ_NONE;
    }
}

bool is_sq_attacked(const Board *board, const Square sq, const Color attacker) {
    const U8 p_offset = attacker * PIECE_PER_SIDE;
    const Bitboard occ = board->occupied[COLOR_BOTH];

    if (pawn_attacks[attacker ^ 1][sq] & board->pieces[p_offset + WHITE_PAWN])
        return true;

    if (knight_attacks[sq] & board->pieces[p_offset + WHITE_KNIGHT])
        return true;

    if (king_attacks[sq] & board->pieces[p_offset + WHITE_KING])
        return true;

    const Bitboard diag_attackers = board->pieces[p_offset + WHITE_BISHOP] |
                              board->pieces[p_offset + WHITE_QUEEN];
    if (get_bishop_attacks(sq, occ) & diag_attackers)
        return true;

    const Bitboard line_attackers = board->pieces[p_offset + WHITE_ROOK] |
                              board->pieces[p_offset + WHITE_QUEEN];
    if (get_rook_attacks(sq, occ) & line_attackers)
        return true;

    return false;
}

void print_board(const Board* board) {
    printf("\n");

    for (Rank rank = RANK_8; rank <= RANK_8; rank--) {
        printf("  ");
        for (File file = FILE_A; file <= FILE_H; file++) {
            printf("+---");
        }
        printf("+\n");
        printf("  ");
        for (File file = FILE_A; file <= FILE_H; file++) {
            const Square sq = rank_file_to_sq(rank, file);
            const Piece piece = board->mailbox[sq];
            printf("| %c ", piece_symbol_of(piece));
        }

        printf("|  %d \n", rank + 1);
    }

    printf("  ");
    for (File file = FILE_A; file <= FILE_H; file++) {
        printf("+---");
    }
    printf("+\n");
    printf("    a   b   c   d   e   f   g   h\n\n");
    char fen_str[256];
    fen_serialize_board(board, fen_str, sizeof(fen_str));
    printf("FEN: %s\n", fen_str);
}
