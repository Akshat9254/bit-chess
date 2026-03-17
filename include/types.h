#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t Bitboard;
typedef uint16_t Move;
typedef uint64_t ZobristHash;
typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;

typedef enum: U8 {
    COLOR_WHITE, COLOR_BLACK, COLOR_BOTH, COLOR_NB
} Color;

typedef enum: U8 {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB, RANK_NONE
} Rank;

typedef enum: U8 {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB, FILE_NONE
} File;

enum: U8 {
	FILE_NB_LOG = 3,
	FILE_NB_REM_MASK = FILE_NB - 1
};

enum: Bitboard {
    RANK_1_MASK = 0x00000000000000ffULL,
    RANK_3_MASK = 0x0000000000ff0000ULL,
    RANK_6_MASK = 0x0000ff0000000000ULL,
    RANK_8_MASK = 0xff00000000000000ULL,

    NOT_A_FILE = 0xfefefefefefefefeULL,
    NOT_H_FILE = 0x7f7f7f7f7f7f7f7fULL
};

typedef enum: U8 {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, 
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, 
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, 
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, 
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, 
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, 
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, 
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NB, SQ_NONE
} Square;

typedef enum: U8 {
	WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    PIECE_NB, PIECE_NONE
} Piece;

typedef enum: U8 {
    PT_PAWN, PT_KNIGHT, PT_BISHOP, PT_ROOK, PT_QUEEN, PT_KING, PIECE_PER_SIDE
} PieceType;

typedef enum: U8 {
    CASTLE_NONE = 0,
    CASTLE_WHITE_OO    = 1,      // 0001 - White Kingside
    CASTLE_WHITE_OOO   = 2,      // 0010 - White Queenside
    CASTLE_BLACK_OO    = 4,      // 0100 - Black Kingside
    CASTLE_BLACK_OOO   = 8,      // 1000 - Black Queenside

    CASTLE_WHITE_ANY   = CASTLE_WHITE_OO | CASTLE_WHITE_OOO, // 0011
    CASTLE_BLACK_ANY   = CASTLE_BLACK_OO | CASTLE_BLACK_OOO, // 1100
    CASTLE_ANY = CASTLE_WHITE_ANY | CASTLE_BLACK_ANY // 1111
} CastlingRights;

#endif
