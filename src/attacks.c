#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "square.h"
#include "attacks.h"
#include "board.h"

Bitboard knight_attacks[SQ_NB];
Bitboard king_attacks[SQ_NB];
Bitboard pawn_attacks[COLOR_NB][SQ_NB];

static void init_knight_attack_table(void);
static void init_king_attack_table(void);
static void init_pawn_attack_table(void);
static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size);

static inline Bitboard get_sliding_piece_attacks(Square sq, Bitboard occ, const Magic *magics);

void init_attack_tables(void) {
    init_knight_attack_table();
    init_king_attack_table();
}

inline Bitboard get_bishop_attacks(const Square sq, const Bitboard occ) {
    return get_sliding_piece_attacks(sq, occ, bishop_magics);
}

inline Bitboard get_rook_attacks(const Square sq, const Bitboard occ) {
    return get_sliding_piece_attacks(sq, occ, rook_magics);
}

static inline Bitboard get_sliding_piece_attacks(Square sq, Bitboard occ, const Magic *magics) {
    const Magic *magic = &magics[sq];
    occ &= magic->relevant_attacks;
    occ *= magic->magic_no;
    occ >>= magic->shift;

    return sliding_pieces_attack_table[magic->attack_offset + occ];
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
