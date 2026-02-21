#ifndef ATTACKS_H
#define ATTACKS_H

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

extern Bitboard knight_attacks[SQ_NB];
extern Bitboard king_attacks[SQ_NB];
extern Bitboard pawn_attacks[COLOR_NB][SQ_NB];

extern Magic bishop_magics[SQ_NB];
extern Magic rook_magics[SQ_NB];
extern Bitboard sliding_pieces_attack_table[ATTACK_TABLE_SIZE];

void init_attack_tables(void);
Bitboard get_bishop_attacks(const Square sq, const Bitboard occ);
Bitboard get_rook_attacks(const Square sq, const Bitboard occ);

#endif
