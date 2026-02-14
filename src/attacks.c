#include <stdbool.h>
#include "attacks.h"
#include "board.h"

bool is_initialized = false;

Bitboard knight_attacks[SQ_NB];

static void init_knight_attack_table(void);

void init_attack_tables(void) {
    if (is_initialized) {
        return;
    }

    init_knight_attack_table();
    is_initialized = true;
}

static void init_knight_attack_table(void) {
    const int8_t offsets[8][2] = {
        { -2, -1 }, { -1, -2 }, { 1, -2 }, { 2, -1 }, 
        { 2, 1 }, { 1, 2 }, { -1, 2 }, { -2, 1 }
    };

    for (Square sq = 0; sq < SQ_NB; sq++) {
        Bitboard attack_mask = 0ULL;
        const uint8_t rank = rank_of_sq(sq);
        const uint8_t file = file_of_sq(sq);

        for (int i = 0; i < 8; i++) {
            int8_t to_rank = rank + offsets[i][0];
            int8_t to_file = file + offsets[i][1];

            if (to_rank >= 0 && to_rank < RANK_NB && to_file >= 0 && to_file < FILE_NB) {
                bb_set(&attack_mask, to_sq(to_rank, to_file));
            }
        }

        knight_attacks[sq] = attack_mask;
    }
}
