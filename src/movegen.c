#include "movegen.h"

#include "leaper_attack_tables.h"
#include "magic_attack_tables.h"

static const Bitboard PAWN_PROMO_RANK_MASK[COLOR_BOTH] = { RANK_8_MASK, RANK_1_MASK };
static const Bitboard PAWN_SINGLE_PUSH_RANK_MASK[COLOR_BOTH] = { RANK_3_MASK, RANK_6_MASK };
static const int8_t PAWN_FORWARD_OFFSET[COLOR_BOTH] = { 8, -8 };
static const int8_t PAWN_LEFT_CAP_OFFSET[COLOR_BOTH] = { 7, -7 };
static const int8_t PAWN_RIGHT_CAP_OFFSET[COLOR_BOTH] = { 9, -9 };

static inline Bitboard pawn_push(Bitboard pawns, Color color);
static inline Bitboard pawn_left_cap(Bitboard pawns, Color color);
static inline Bitboard pawn_right_cap(Bitboard pawns, Color color);

static void filter_legal_moves(Board *board, const MoveList *pseudo_legal_moves, MoveList *legal_moves);
static void generate_pawn_quiet_push_moves(const Board *board, MoveList *pseudo_legal_moves);
static void generate_pawn_promo_and_capture_moves(const Board *board, MoveList *pseudo_legal_moves);
static void generate_pawn_en_passant_moves(const Board *board, MoveList *pseudo_legal_moves);

void generate_all_legal_moves(Board *board, MoveList *legal_moves) {
    MoveList pseudo_legal_moves = {0};
    const Bitboard not_us = ~board->occupied[board->side_to_move];

    generate_all_pawn_pseudo_legal_moves(board, &pseudo_legal_moves);
    generate_all_knight_pseudo_legal_moves(board, &pseudo_legal_moves, not_us);
    generate_all_bishop_pseudo_legal_moves(board, &pseudo_legal_moves, not_us);
    generate_all_rook_pseudo_legal_moves(board, &pseudo_legal_moves, not_us);
    generate_all_queen_pseudo_legal_moves(board, &pseudo_legal_moves, not_us);
    generate_all_king_pseudo_legal_moves(board, &pseudo_legal_moves, not_us);

    filter_legal_moves(board, &pseudo_legal_moves, legal_moves);
}

void generate_all_legal_capture_moves(Board *board, MoveList *capture_moves) {
    MoveList pseudo_legal_moves = {0};
    const Bitboard enemy_mask = board->occupied[board->side_to_move ^ 1];

    generate_all_pawn_pseudo_legal_capture_moves(board, &pseudo_legal_moves);
    generate_all_knight_pseudo_legal_moves(board, &pseudo_legal_moves, enemy_mask);
    generate_all_bishop_pseudo_legal_moves(board, &pseudo_legal_moves, enemy_mask);
    generate_all_rook_pseudo_legal_moves(board, &pseudo_legal_moves, enemy_mask);
    generate_all_queen_pseudo_legal_moves(board, &pseudo_legal_moves, enemy_mask);
    generate_all_king_pseudo_legal_moves(board, &pseudo_legal_moves, enemy_mask);

    filter_legal_moves(board, &pseudo_legal_moves, capture_moves);
}

void generate_all_pawn_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves) {
    generate_pawn_quiet_push_moves(board, pseudo_legal_moves);
    generate_pawn_promo_and_capture_moves(board, pseudo_legal_moves);
    generate_pawn_en_passant_moves(board, pseudo_legal_moves);
}

void generate_all_pawn_pseudo_legal_capture_moves(const Board *board, MoveList *pseudo_legal_moves) {
    generate_pawn_promo_and_capture_moves(board, pseudo_legal_moves);
    generate_pawn_en_passant_moves(board, pseudo_legal_moves);
}

void generate_all_knight_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, const Bitboard filter_mask) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard enemy_mask = board->occupied[them];
    const bool is_cap_only = filter_mask == enemy_mask;

    Bitboard knights = board->pieces[WHITE_KNIGHT + piece_color_offset];

    while (knights) {
        const Square from = bb_find_lssb_index(knights);
        bb_pop_lssb(&knights);

        Bitboard moves = knight_attacks[from] & filter_mask;
        while (moves) {
            const Square to = bb_find_lssb_index(moves);
            bb_pop_lssb(&moves);

            const U8 flag = is_cap_only || bb_test(enemy_mask, to) ? MOVE_CAPTURE : MOVE_QUIET;
            const Move move = create_move(from, to, flag);
            add_move(pseudo_legal_moves, move);
        }
    }
}

void generate_all_bishop_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, const Bitboard filter_mask) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard enemy_mask = board->occupied[them];
    const Bitboard all_occ_mask = board->occupied[COLOR_BOTH];
    const bool is_cap_only = filter_mask == enemy_mask;

    Bitboard bishops = board->pieces[WHITE_BISHOP + piece_color_offset];

    while (bishops) {
        const Square from = bb_find_lssb_index(bishops);
        bb_pop_lssb(&bishops);

        Bitboard moves = get_bishop_attacks(from, all_occ_mask) & filter_mask;

        while (moves) {
            const Square to = bb_find_lssb_index(moves);
            bb_pop_lssb(&moves);

            const U8 flag = is_cap_only || bb_test(enemy_mask, to) ? MOVE_CAPTURE : MOVE_QUIET;
            const Move move = create_move(from, to, flag);
            add_move(pseudo_legal_moves, move);
        }
    }
}

void generate_all_rook_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, const Bitboard filter_mask) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard enemy_mask = board->occupied[them];
    const Bitboard all_occ_mask = board->occupied[COLOR_BOTH];
    const bool is_cap_only = filter_mask == enemy_mask;

    Bitboard rooks = board->pieces[WHITE_ROOK + piece_color_offset];

    while (rooks) {
        const Square from = bb_find_lssb_index(rooks);
        bb_pop_lssb(&rooks);

        Bitboard moves = get_rook_attacks(from, all_occ_mask) & filter_mask;

        while (moves) {
            const Square to = bb_find_lssb_index(moves);
            bb_pop_lssb(&moves);

            const U8 flag = is_cap_only || bb_test(enemy_mask, to) ? MOVE_CAPTURE : MOVE_QUIET;
            const Move move = create_move(from, to, flag);
            add_move(pseudo_legal_moves, move);
        }
    }
}

void generate_all_queen_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, const Bitboard filter_mask) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard enemy_mask = board->occupied[them];
    const Bitboard all_occ_mask = board->occupied[COLOR_BOTH];
    const bool is_cap_only = filter_mask == enemy_mask;

    Bitboard queens = board->pieces[WHITE_QUEEN + piece_color_offset];

    while (queens) {
        const Square from = bb_find_lssb_index(queens);
        bb_pop_lssb(&queens);

        Bitboard moves = (get_bishop_attacks(from, all_occ_mask) | get_rook_attacks(from, all_occ_mask)) & filter_mask;

        while (moves) {
            const Square to = bb_find_lssb_index(moves);
            bb_pop_lssb(&moves);

            const U8 flag = is_cap_only || bb_test(enemy_mask, to) ? MOVE_CAPTURE : MOVE_QUIET;
            const Move move = create_move(from, to, flag);
            add_move(pseudo_legal_moves, move);
        }
    }
}

void generate_all_king_pseudo_legal_moves(const Board *board, MoveList *pseudo_legal_moves, const Bitboard filter_mask) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const Bitboard enemy_mask = board->occupied[them];
    const Bitboard all_occ_mask = board->occupied[COLOR_BOTH];
    const bool is_cap_only = filter_mask == enemy_mask;

    const Square from = board->kings_sq[us];

    Bitboard moves = king_attacks[from] & filter_mask;
    while (moves) {
        const Square to = bb_find_lssb_index(moves);
        bb_pop_lssb(&moves);

        const U8 flag = is_cap_only || bb_test(enemy_mask, to) ? MOVE_CAPTURE : MOVE_QUIET;
        const Move move = create_move(from, to, flag);
        add_move(pseudo_legal_moves, move);
    }

    if (filter_mask == enemy_mask || is_king_in_check(board, us)) {
        return;
    }

    if (us == COLOR_WHITE) {
        if (board->castling_rights & CASTLE_WHITE_OO) {
            if (!bb_test(all_occ_mask, SQ_F1) &&
                !bb_test(all_occ_mask, SQ_G1) &&
                !is_sq_attacked(board, SQ_F1, COLOR_BLACK) &&
                !is_sq_attacked(board, SQ_G1, COLOR_BLACK)) {
                const Move move = create_move(from, SQ_G1, MOVE_KING_CASTLE);
                add_move(pseudo_legal_moves, move);
            }
        }

        if (board->castling_rights & CASTLE_WHITE_OOO) {
            if (!bb_test(all_occ_mask, SQ_D1) &&
                !bb_test(all_occ_mask, SQ_C1) &&
                !bb_test(all_occ_mask, SQ_B1) &&
                !is_sq_attacked(board, SQ_D1, COLOR_BLACK) &&
                !is_sq_attacked(board, SQ_C1, COLOR_BLACK)) {
                const Move move = create_move(from, SQ_C1, MOVE_QUEEN_CASTLE);
                add_move(pseudo_legal_moves, move);
            }
        }
    } else {
        if (board->castling_rights & CASTLE_BLACK_OO) {
            if (!bb_test(all_occ_mask, SQ_F8) &&
                !bb_test(all_occ_mask, SQ_G8) &&
                !is_sq_attacked(board, SQ_F8, COLOR_WHITE) &&
                !is_sq_attacked(board, SQ_G8, COLOR_WHITE)) {
                const Move move = create_move(from, SQ_G8, MOVE_KING_CASTLE);
                add_move(pseudo_legal_moves, move);
            }
        }

        if (board->castling_rights & CASTLE_BLACK_OOO) {
            if (!bb_test(all_occ_mask, SQ_D8) &&
                !bb_test(all_occ_mask, SQ_C8) &&
                !bb_test(all_occ_mask, SQ_B8) &&
                !is_sq_attacked(board, SQ_D8, COLOR_WHITE) &&
                !is_sq_attacked(board, SQ_C8, COLOR_WHITE)) {
                const Move move = create_move(from, SQ_C8, MOVE_QUEEN_CASTLE);
                add_move(pseudo_legal_moves, move);
            }
        }
    }
}

static void generate_pawn_quiet_push_moves(const Board *board, MoveList *pseudo_legal_moves) {
    const Color us = board->side_to_move;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard pawns = board->pieces[WHITE_PAWN + piece_color_offset];
    const Bitboard empty_mask = ~board->occupied[COLOR_BOTH];

    const Bitboard single_push = pawn_push(pawns, us) & empty_mask;
    Bitboard quiet = (single_push & ~PAWN_PROMO_RANK_MASK[us]);

    while (quiet) {
        const Square to = bb_find_lssb_index(quiet);
        bb_pop_lssb(&quiet);
        const Square from = to - PAWN_FORWARD_OFFSET[us];
        const Move move = create_move(from, to, MOVE_QUIET);
        add_move(pseudo_legal_moves, move);
    }

    Bitboard double_push = pawn_push(single_push & PAWN_SINGLE_PUSH_RANK_MASK[us], us) & empty_mask;
    while (double_push) {
        const Square to = bb_find_lssb_index(double_push);
        bb_pop_lssb(&double_push);
        const Square from = to - 2 * PAWN_FORWARD_OFFSET[us];
        const Move move = create_move(from, to, MOVE_PAWN_DOUBLE_PUSH);
        add_move(pseudo_legal_moves, move);
    }
}

static void generate_pawn_promo_and_capture_moves(const Board *board, MoveList *pseudo_legal_moves) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;

    const Bitboard pawns = board->pieces[WHITE_PAWN + piece_color_offset];
    const Bitboard empty_mask = ~board->occupied[COLOR_BOTH];
    const Bitboard enemy_mask = board->occupied[them];

    const Bitboard single_push = pawn_push(pawns, us) & empty_mask;
    Bitboard promo = (single_push & PAWN_PROMO_RANK_MASK[us]);

    while (promo) {
        const Square to = bb_find_lssb_index(promo);
        bb_pop_lssb(&promo);
        const Square from = to - PAWN_FORWARD_OFFSET[us];
        for (U8 promo_flag = MOVE_PROMO_N; promo_flag <= MOVE_PROMO_Q; promo_flag++) {
            const Move move = create_move(from, to, promo_flag);
            add_move(pseudo_legal_moves, move);
        }
    }

    const Bitboard left_captures = pawn_left_cap(pawns, us) & enemy_mask;
    Bitboard left_captures_without_promo = left_captures & ~PAWN_PROMO_RANK_MASK[us];
    Bitboard left_captures_with_promo = left_captures & PAWN_PROMO_RANK_MASK[us];

    while (left_captures_without_promo) {
        const Square to = bb_find_lssb_index(left_captures_without_promo);
        bb_pop_lssb(&left_captures_without_promo);
        const Square from = to - PAWN_LEFT_CAP_OFFSET[us];
        const Move move = create_move(from, to, MOVE_CAPTURE);
        add_move(pseudo_legal_moves, move);
    }

    while (left_captures_with_promo) {
        const Square to = bb_find_lssb_index(left_captures_with_promo);
        bb_pop_lssb(&left_captures_with_promo);
        const Square from = to - PAWN_LEFT_CAP_OFFSET[us];
        for (U8 promo_flag = MOVE_PROMO_N_CAP; promo_flag <= MOVE_PROMO_Q_CAP; promo_flag++) {
            const Move move = create_move(from, to, promo_flag);
            add_move(pseudo_legal_moves, move);
        }
    }

    const Bitboard right_captures = pawn_right_cap(pawns, us) & enemy_mask;
    Bitboard right_captures_without_promo = right_captures & ~PAWN_PROMO_RANK_MASK[us];
    Bitboard right_captures_with_promo = right_captures & PAWN_PROMO_RANK_MASK[us];

    while (right_captures_without_promo) {
        const Square to = bb_find_lssb_index(right_captures_without_promo);
        bb_pop_lssb(&right_captures_without_promo);
        const Square from = to - PAWN_RIGHT_CAP_OFFSET[us];
        const Move move = create_move(from, to, MOVE_CAPTURE);
        add_move(pseudo_legal_moves, move);
    }

    while (right_captures_with_promo) {
        const Square to = bb_find_lssb_index(right_captures_with_promo);
        bb_pop_lssb(&right_captures_with_promo);
        const Square from = to - PAWN_RIGHT_CAP_OFFSET[us];
        for (U8 promo_flag = MOVE_PROMO_N_CAP; promo_flag <= MOVE_PROMO_Q_CAP; promo_flag++) {
            const Move move = create_move(from, to, promo_flag);
            add_move(pseudo_legal_moves, move);
        }
    }
}

static void generate_pawn_en_passant_moves(const Board *board, MoveList *pseudo_legal_moves) {
    const Color us = board->side_to_move;
    const Color them = us ^ 1;
    const U8 piece_color_offset = us * PIECE_PER_SIDE;
    const Bitboard pawns = board->pieces[WHITE_PAWN + piece_color_offset];

    if (board->enpassant_sq != SQ_NONE) {
        Bitboard attackers = pawn_attacks[them][board->enpassant_sq] & pawns;

        while (attackers) {
            const Square from = bb_find_lssb_index(attackers);
            bb_pop_lssb(&attackers);
            const Move move = create_move(from, board->enpassant_sq, MOVE_ENPASSANT);
            add_move(pseudo_legal_moves, move);
        }
    }
}

static void filter_legal_moves(Board *board, const MoveList *pseudo_legal_moves, MoveList *legal_moves) {
    for (size_t i = 0; i < pseudo_legal_moves->count; i++) {
        const Move move = pseudo_legal_moves->moves[i];
        StateInfo state;

        make_move(board, move, &state);

        if (!is_king_in_check(board, board->side_to_move ^ 1)) {
            legal_moves->moves[legal_moves->count++] = move;
        }

        unmake_move(board, &state);
    }
}

static inline Bitboard pawn_push(const Bitboard pawns, const Color color) {
    return color == COLOR_WHITE ? (pawns << 8) : (pawns >> 8);
}

static inline Bitboard pawn_left_cap(const Bitboard pawns, const Color color) {
    return color == COLOR_WHITE ? ((pawns & NOT_A_FILE) << 7) : ((pawns & NOT_H_FILE) >> 7);
}

static inline Bitboard pawn_right_cap(const Bitboard pawns, const Color color) {
    return color == COLOR_WHITE ? ((pawns & NOT_H_FILE) << 9) : ((pawns & NOT_A_FILE) >> 9);
}
