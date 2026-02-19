#include <stdlib.h>
#include "square.h"

uint8_t rank_dist(Square sq1, Square sq2) {
	int8_t dist = rank_of_sq(sq1) - rank_of_sq(sq2);
	return abs(dist);
}

uint8_t file_dist(Square sq1, Square sq2) {
	int8_t dist = file_of_sq(sq1) - file_of_sq(sq2);
	return abs(dist);
}
