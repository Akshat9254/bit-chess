#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "attacks.h"
#include "board.h"

bool is_initialized = false;

Bitboard knight_attacks[SQ_NB];
Bitboard king_attacks[SQ_NB];
Bitboard pawn_attacks[COLOR_NB][SQ_NB];

Bitboard bishop_relevant_attacks[SQ_NB];
Bitboard rook_relevant_attacks[SQ_NB];

static void init_knight_attack_table(void);
static void init_king_attack_table(void);
static void init_pawn_attack_table(void);
static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size);
static void init_bishop_relevant_attacks(void);
static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq);
static void init_rook_relevant_attacks(void);
static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq);

void init_attack_tables(void) {
    if (is_initialized) {
        return;
    }

    init_knight_attack_table();
    init_king_attack_table();
    init_bishop_relevant_attacks();
    init_rook_relevant_attacks();
    is_initialized = true;
}

static void init_knight_attack_table(void) {
    const Offset offsets[8] = {
        { -2, -1 }, { -1, -2 }, { 1, -2 }, { 2, -1 }, 
        { 2, 1 }, { 1, 2 }, { -1, 2 }, { -2, 1 }
    };

    init_leaper_attacks(knight_attacks, offsets, 8);
}

static void init_king_attack_table(void) {
    const Offset offsets[8] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
        { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }
    };

    init_leaper_attacks(king_attacks, offsets, 8);
}

static void init_pawn_attack_table(void) {
    const Offset white_offsets[2] = {{ 1, -1 }, { 1, 1 }};
    const Offset black_offsets[2] = {{ -1, -1 }, { -1, 1 }};

    init_leaper_attacks(pawn_attacks[WHITE], white_offsets, 2);
    init_leaper_attacks(pawn_attacks[BLACK], black_offsets, 2);
}

static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        Bitboard attack_mask = 0ULL;
        const uint8_t rank = rank_of_sq(sq);
        const uint8_t file = file_of_sq(sq);

        for (uint8_t i = 0; i < offset_size; i++) {
            int8_t to_rank = rank + offsets[i].delta_rank;
            int8_t to_file = file + offsets[i].delta_file;

            if (is_rank_file_on_board(to_rank, to_file)) {
                bb_set(&attack_mask, to_sq(to_rank, to_file));
            }
        }

        attack_table[sq] = attack_mask;
    }
}

static void init_bishop_relevant_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        bishop_relevant_attacks[sq] = generate_bishop_relevant_attacks_from_sq(sq);
    }
}

static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1, to_file = file - 1; to_rank > 0 && to_file > 0; to_rank--, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank - 1, to_file = file + 1; to_rank > 0 && to_file < FILE_H_INDEX; to_rank--, to_file++) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1, to_file = file - 1; to_rank < RANK_NB - 1 && to_file > 0; to_rank++, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1, to_file = file + 1; to_rank < RANK_NB - 1 && to_file < FILE_H_INDEX; to_rank++, to_file++) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    return attack_mask;
}

static void init_rook_relevant_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        rook_relevant_attacks[sq] = generate_rook_relevant_attacks_from_sq(sq);
    }
}

static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1; to_rank > 0; to_rank--) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_file = file - 1; to_file > 0; to_file--) {
        Square to = to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1; to_rank < RANK_NB - 1; to_rank++) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_file = file + 1; to_file < FILE_H_INDEX; to_file++) {
        Square to = to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    return attack_mask;
}
