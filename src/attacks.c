#include <stdbool.h>
#include <stddef.h>
#include "attacks.h"
#include "board.h"

bool is_initialized = false;

Bitboard knight_attacks[SQ_NB];
Bitboard king_attacks[SQ_NB];

static void init_knight_attack_table(void);
static void init_king_attack_table(void);
static void add_attack_masks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size);

void init_attack_tables(void) {
    if (is_initialized) {
        return;
    }

    init_knight_attack_table();
    init_king_attack_table();
    is_initialized = true;
}

static void init_knight_attack_table(void) {
    const Offset offsets[8] = {
        { -2, -1 }, { -1, -2 }, { 1, -2 }, { 2, -1 }, 
        { 2, 1 }, { 1, 2 }, { -1, 2 }, { -2, 1 }
    };

    add_attack_masks(knight_attacks, offsets, 8);
}

static void init_king_attack_table(void) {
    const Offset offsets[8] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
        { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }
    };

    add_attack_masks(king_attacks, offsets, 8);
}

static void add_attack_masks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        Bitboard attack_mask = 0ULL;
        const uint8_t rank = rank_of_sq(sq);
        const uint8_t file = file_of_sq(sq);

        for (uint8_t i = 0; i < offset_size; i++) {
            int8_t to_rank = rank + offsets[i].delta_rank;
            int8_t to_file = file + offsets[i].delta_file;

            if (to_rank >= 0 && to_rank < RANK_NB && to_file >= 0 && to_file < FILE_NB) {
                bb_set(&attack_mask, to_sq(to_rank, to_file));
            }
        }

        attack_table[sq] = attack_mask;
    }
}

