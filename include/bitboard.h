#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t Bitboard;

void bb_print(Bitboard bb);
bool bb_test(Bitboard bb, int sq);
void bb_set(Bitboard *bb, int sq);
void bb_clear(Bitboard *bb, int sq);

#endif

