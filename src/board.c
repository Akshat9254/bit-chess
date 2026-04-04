#include <assert.h>
#include <stdio.h>

#include "board.h"
#include "fen_parser.h"
#include "leaper_attack_tables.h"
#include "magic_attack_tables.h"
#include "move.h"
#include "piece.h"
#include "zobrist.h"

static const bool is_king_piece[PIECE_NB] = {
    [WHITE_KING] = true, [BLACK_KING] = true
};

static const struct { Square from, to; } CASTLE_ROOK_MOVES[COLOR_BOTH][2] = {
    { {SQ_H1, SQ_F1}, {SQ_A1, SQ_D1} },
    { {SQ_H8, SQ_F8}, {SQ_A8, SQ_D8} }
};

static const U8 CASTLE_RIGHTS_MASK[SQ_NB] = {
    0x0d, 0x0f, 0x0f, 0x0f, 0x0c, 0x0f, 0x0f, 0x0e,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
    0x07, 0x0f, 0x0f, 0x0f, 0x03, 0x0f, 0x0f, 0x0b
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

void reset_board(Board *board) {
    FenError err;
    const bool result = fen_init_board(board, FEN_START_POS, &err);
    if (!result) {
        perror(err.message);
    }
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
    board->hash ^= ZOBRIST_PIECE_KEYS[piece][sq];

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
    board->hash ^= ZOBRIST_PIECE_KEYS[piece][sq];

    if (is_king_piece[piece]) {
        board->kings_sq[piece_color] = SQ_NONE;
    }
}

void make_move(Board *board, const Move move, StateInfo *state) {
    const Square from = move_get_from(move);
    const Square to = move_get_to(move);
    const U8 flags = move_get_flags(move);
    const Piece moved_piece = board->mailbox[from];
    const Color us = board->side_to_move;

    assert(from != SQ_NONE);
    assert(flags <= MOVE_PROMO_Q_CAP);
    assert(moved_piece != PIECE_NONE);

    state->move = move;
    state->castling_rights = board->castling_rights;
    state->en_passant_sq = board->enpassant_sq;
    state->half_move_clock = board->half_move_clock;
    state->captured_piece = PIECE_NONE;

    if (is_capture(flags)) {
        const Square cap_sq = (flags == MOVE_ENPASSANT) ? (us == COLOR_WHITE ? to - 8 : to + 8) : to;
        const Piece cap_piece = board->mailbox[cap_sq];
        state->captured_piece = cap_piece;
        clear_sq(board, cap_sq);
    }

    clear_sq(board, from);

    if (is_promo(flags)) {
        const Piece promo_piece = move_get_promo_piece(flags, us);
        place_piece_on_sq(board, promo_piece, to);
    } else {
        place_piece_on_sq(board, moved_piece, to);
    }

    if (is_castle(flags)) {
        const bool is_queen_side_castle = flags == MOVE_QUEEN_CASTLE;
        const Square rook_from = CASTLE_ROOK_MOVES[us][is_queen_side_castle].from;
        const Square rook_to = CASTLE_ROOK_MOVES[us][is_queen_side_castle].to;
        const Piece rook = board->mailbox[rook_from];
        move_piece(board, rook, rook_from, rook_to);
    }

    board->castling_rights &= CASTLE_RIGHTS_MASK[from] & CASTLE_RIGHTS_MASK[to];
    board->enpassant_sq = (flags == MOVE_PAWN_DOUBLE_PUSH) ? (from + to) >> 1 : SQ_NONE;
    board->side_to_move ^= 1;
    if (is_capture(flags) || moved_piece == WHITE_PAWN || moved_piece == BLACK_PAWN) {
        board->half_move_clock = 0;
    } else {
        board->half_move_clock++;
    }

    if (us == COLOR_BLACK) {
        board->full_move_number++;
    }

    board->hash ^= ZOBRIST_CASTLING_KEYS[state->castling_rights] ^ ZOBRIST_CASTLING_KEYS[board->castling_rights];
    board->hash ^= ZOBRIST_EN_PASSANT_KEYS[state->en_passant_sq] ^ ZOBRIST_EN_PASSANT_KEYS[board->enpassant_sq];
    board->hash ^= ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_WHITE] ^ ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_BLACK];
}

void unmake_move(Board *board, const StateInfo *state) {
    const Move move = state->move;
    const Square from = move_get_from(move);
    const Square to = move_get_to(move);
    const U8 flags = move_get_flags(move);

    const Piece moved_piece = board->mailbox[to];
    const Color moved_piece_color = board->side_to_move ^ 1;

    clear_sq(board, to);

    if (is_promo(flags)) {
        const Piece pawn = WHITE_PAWN + moved_piece_color * PIECE_PER_SIDE;
        place_piece_on_sq(board, pawn, from);
    } else {
        place_piece_on_sq(board, moved_piece, from);
    }

    if (is_capture(flags)) {
        const Square cap_sq = (flags == MOVE_ENPASSANT) ? (moved_piece_color == COLOR_WHITE ? to - 8 : to + 8) : to;
        const Piece cap_piece = state->captured_piece;
        place_piece_on_sq(board, cap_piece, cap_sq);
    }

    if (is_castle(flags)) {
        const bool is_queen_side_castle = flags == MOVE_QUEEN_CASTLE;
        const Square rook_from = CASTLE_ROOK_MOVES[moved_piece_color][is_queen_side_castle].from;
        const Square rook_to = CASTLE_ROOK_MOVES[moved_piece_color][is_queen_side_castle].to;
        const Piece rook = WHITE_ROOK + moved_piece_color * PIECE_PER_SIDE;
        move_piece(board, rook, rook_to, rook_from);
    }

    board->hash ^= ZOBRIST_CASTLING_KEYS[state->castling_rights] ^ ZOBRIST_CASTLING_KEYS[board->castling_rights];
    board->hash ^= ZOBRIST_EN_PASSANT_KEYS[state->en_passant_sq] ^ ZOBRIST_EN_PASSANT_KEYS[board->enpassant_sq];
    board->hash ^= ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_WHITE] ^ ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_BLACK];

    board->castling_rights = state->castling_rights;
    board->enpassant_sq = state->en_passant_sq;
    board->side_to_move = moved_piece_color;
    board->half_move_clock = state->half_move_clock;

    if (moved_piece_color == COLOR_BLACK) {
        board->full_move_number--;
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
