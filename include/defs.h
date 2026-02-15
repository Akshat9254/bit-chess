#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

typedef uint64_t Bitboard;

#define RANK_NB 8
#define FILE_NB 8
#define FILE_NB_LOG 3
#define FILE_NB_REM_MASK 7
#define PIECES_PER_SIDE 6

#define RANK_2_MASK 0x000000000000FF00ULL
#define RANK_7_MASK 0x00FF000000000000ULL
#define FILE_A 'a'
#define FILE_H 'h'

typedef enum {
	WHITE,
	BLACK,
	COLOR_NB
} Color;

typedef enum {
	WHITE_PAWN,
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_ROOK,
	WHITE_QUEEN,
	WHITE_KING,
	BLACK_PAWN,
	BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_ROOK,
	BLACK_QUEEN,
	BLACK_KING,
	PIECE_NB,
	NO_PIECE
} Piece;

typedef enum {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	SQ_NB, NO_SQ
} Square;

typedef struct {
	int8_t delta_rank;
	int8_t delta_file;
} Offset;

#define ORTHOGONAL_OFFSETS_SIZE 4
#define DIAGONAL_OFFSETS_SIZE 4

extern const Offset orthogonal_offsets[];
extern const Offset diagonal_offsets[];
extern const char* piece_symbols;

#endif
