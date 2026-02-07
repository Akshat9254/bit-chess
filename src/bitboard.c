#include <string.h>
#include <stdio.h>
#include "bitboard.h"
#include "board.h"

void bb_print(Bitboard bb) {	
	for (int rank = RANK_NB - 1; rank >= 0; rank--) {
		for (int file = 0; file < FILE_NB; file++) {
			int sq = rank * FILE_NB + file;
			int is_occupied = (bb & (1ULL << sq)) > 0;
			printf("%c\t", (is_occupied ? '1': '0'));
		}
		
		printf("\n");
	}
}

inline bool bb_test(Bitboard bb, int sq) {
	return (bb & (1ULL << sq)) > 0;
}

inline void bb_set(Bitboard *bb, int sq) {
	*bb |= (1ULL << sq);
}

inline void bb_clear(Bitboard *bb, int sq) {
	*bb &= ~(1ULL << sq);
}
