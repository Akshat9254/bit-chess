#ifndef MAGIC_ATTACK_TABLES_H
#define MAGIC_ATTACK_TABLES_H

#include "types.h"

enum {
    BISHOP_OCC_COUNT = 5248,
    ROOK_OCC_COUNT = 102400,
    ATTACK_TABLE_SIZE = 107648,
    MAX_BISHOP_OCC = 1 << 9,
    MAX_ROOK_OCC = 1 << 12
};

typedef struct {
    Bitboard relevant_attacks;
    U64 magic_no;
    U32 attack_offset;
    U8 shift;
} Magic;

extern const Magic bishop_magics[SQ_NB];
extern const Magic rook_magics[SQ_NB];
extern const Bitboard sliding_pieces_attack_table[ATTACK_TABLE_SIZE];

static inline Bitboard get_sliding_piece_attacks(const Square sq, Bitboard occ, const Magic *magics) {
    const Magic *magic = &magics[sq];
    occ &= magic->relevant_attacks;
    occ *= magic->magic_no;
    occ >>= magic->shift;

    return sliding_pieces_attack_table[magic->attack_offset + occ];
}

static inline Bitboard get_bishop_attacks(const Square sq, const Bitboard occ) {
    return get_sliding_piece_attacks(sq, occ, bishop_magics);
}

static inline Bitboard get_rook_attacks(const Square sq, const Bitboard occ) {
    return get_sliding_piece_attacks(sq, occ, rook_magics);
}

#endif
