#ifndef ATTACKS_H
#define ATTACKS_H

#include "chess_types.h"
#include "magic_tables.h"

extern Bitboard knight_attacks[SQ_NB];
extern Bitboard king_attacks[SQ_NB];
extern Bitboard pawn_attacks[COLOR_NB][SQ_NB];

void init_attack_tables(void);
Bitboard get_bishop_attacks(const Square sq, const Bitboard occ);
Bitboard get_rook_attacks(const Square sq, const Bitboard occ);

#endif
