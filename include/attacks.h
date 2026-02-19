#ifndef ATTACKS_H
#define ATTACKS_H

#include "chess_types.h"

extern Bitboard knight_attacks[SQ_NB];
extern Bitboard king_attacks[SQ_NB];
extern Bitboard pawn_attacks[COLOR_NB][SQ_NB];
extern Bitboard bishop_relevant_attacks[SQ_NB];
extern Bitboard rook_relevant_attacks[SQ_NB];

void init_attack_tables(void);

#endif
