#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <stdint.h>

#define RANK_2_MASK 0x000000000000FF00ULL
#define RANK_7_MASK 0x00FF000000000000ULL

typedef uint64_t Bitboard;
typedef uint64_t U64;
typedef uint32_t U32;
typedef uint8_t U8;

enum {
	FILE_A, FILE_B, FILE_C, FILE_D, 
	FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum {
	RANK_1, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

enum {
	FILE_NB_LOG = 3,
	FILE_NB_REM_MASK = FILE_NB - 1
};

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

enum {
	PIECES_PER_SIDE = 6
};

typedef enum {
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8,
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

#endif
