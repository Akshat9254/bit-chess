#ifndef ATTACKS_H
#define ATTACKS_H

#include "defs.h"

extern Bitboard knight_attacks[SQ_NB];
extern Bitboard king_attacks[SQ_NB];
extern Bitboard pawn_attacks[COLOR_NB][SQ_NB];

void init_attack_tables(void);

#endif
