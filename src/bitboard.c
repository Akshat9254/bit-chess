#include <inttypes.h>
#include <stdio.h>

#include "bitboard.h"
#include "square.h"

void bb_print(const Bitboard bb) {
	printf("\n");
	for (Rank rank = RANK_8; rank <= RANK_8; rank--) {
		printf("  %d  ", rank + 1);
		for (File file = FILE_A; file <= FILE_H; file++) {
            const Square sq = rank_file_to_sq(rank, file);
			printf("%d  ", bb_test(bb, sq));
		}
		
		printf("\n");
	}

	printf("     a  b  c  d  e  f  g  h\n\n");
	printf("  Bitboard = 0x%016" PRIx64 "ULL\n\n", bb);
}
