#include <string.h>
#include <stdio.h>
#include "bitboard.h"
#include "board.h"

void bb_print(Bitboard bb) {	
	printf("\n");
	for (int rank = RANK_NB - 1; rank >= 0; rank--) {
		for (int file = 0; file < FILE_NB; file++) {
			int sq = rank * FILE_NB + file;
			int is_occupied = (bb & (1ULL << sq)) > 0;
			if (file == 0) {
				printf("  %d  ", rank + 1);
			}
			printf("%d  ", (is_occupied ? 1: 0));
		}
		
		printf("\n");
	}

	printf("     a  b  c  d  e  f  g  h\n\n");
	printf("  Bitboard = 0x%016llxULL\n\n", bb);
}

Square pop_lssb(Bitboard *bb) {
	if (*bb == 0) {
        return NO_SQ;
    }

    Square lssb_value = __builtin_ctzll(*bb);
    *bb &= (*bb - 1);

    return lssb_value;
}
