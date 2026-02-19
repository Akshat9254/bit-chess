#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdbool.h>
#include "chess_types.h"

void bb_print(Bitboard bb);

static inline bool bb_test(Bitboard bb, Square sq) {
	return ((bb >> sq) & 1);
}

static inline void bb_set(Bitboard *bb, Square sq) {
	*bb |= (1ULL << sq);
}

static inline void bb_clear(Bitboard *bb, Square sq) {
	*bb &= ~(1ULL << sq);
}

static inline int8_t bb_popcount(Bitboard bb) {
	return __builtin_popcountll(bb);
}

static inline int8_t bb_find_lssb_index(Bitboard bb) {
	return bb == 0 ? -1 : __builtin_ctzll(bb);
}

static inline void bb_pop_lssb(Bitboard *bb) {
	if (*bb == 0) {
        return;
    }

    *bb &= (*bb - 1);
}

#endif

