#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdbool.h>
#include "defs.h"

void bb_print(Bitboard bb);
bool bb_test(Bitboard bb, int sq);
void bb_set(Bitboard *bb, int sq);
void bb_clear(Bitboard *bb, int sq);
Square pop_lssb(Bitboard *bb);

#endif

