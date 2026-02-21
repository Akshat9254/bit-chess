#ifndef MAGIC_TABLES_H
#define MAGIC_TABLES_H

#include "chess_types.h"

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

#endif
