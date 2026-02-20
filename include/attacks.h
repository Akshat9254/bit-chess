#ifndef ATTACKS_H
#define ATTACKS_H

#include "chess_types.h"

#define MAX_BISHOP_OCC (1 << 9)
#define MAX_ROOK_OCC (1 << 12)

extern Bitboard knight_attacks[SQ_NB];
extern Bitboard king_attacks[SQ_NB];
extern Bitboard pawn_attacks[COLOR_NB][SQ_NB];

extern Bitboard bishop_relevant_attacks[SQ_NB];
extern U64 bishop_magics[SQ_NB];
extern U8 bishop_shift[SQ_NB];
extern Bitboard bishop_attacks[SQ_NB][MAX_BISHOP_OCC];

extern Bitboard rook_relevant_attacks[SQ_NB];
extern U64 rook_magics[SQ_NB];
extern U8 rook_shift[SQ_NB];
extern Bitboard rook_attacks[SQ_NB][MAX_ROOK_OCC];

void init_attack_tables(void);

#endif
