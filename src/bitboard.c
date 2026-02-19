#include <string.h>
#include <stdio.h>
#include "bitboard.h"
#include "board.h"

void bb_print(Bitboard bb) {	
	printf("\n");
	for (int rank = RANK_NB - 1; rank >= 0; rank--) {
		for (int file = 0; file < FILE_NB; file++) {
			int sq = rank * FILE_NB + file;
			int is_occupied = bb_test(bb, sq);
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

