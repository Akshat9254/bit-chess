#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

extern const U64 ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_NB];
extern const U64 ZOBRIST_CASTLING_KEYS[CASTLE_ANY + 1];
extern const U64 ZOBRIST_EN_PASSANT_KEYS[SQ_NB + 1];
extern const U64 ZOBRIST_PIECE_KEYS[PIECE_NB][SQ_NB];

#endif
