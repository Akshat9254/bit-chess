#include <stdio.h>
#include <_inttypes.h>

#include "utils.h"
#include "zobrist.h"

static U64 GENERATED_ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_NB];
static U64 GENERATED_ZOBRIST_CASTLING_KEYS[CASTLE_ANY + 1];
static U64 GENERATED_ZOBRIST_EN_PASSANT_KEYS[SQ_NB + 1];
static U64 GENERATED_ZOBRIST_PIECE_KEYS[PIECE_NB][SQ_NB];

static void print_side_to_move_key(void);
static void print_castling_keys(void);
static void print_en_passant_keys(void);
static void print_piece_keys(void);

int main(void) {
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
            GENERATED_ZOBRIST_PIECE_KEYS[piece][sq] = (random_u64() & random_u64() & random_u64());
        }

        GENERATED_ZOBRIST_EN_PASSANT_KEYS[sq] = (random_u64() & random_u64() & random_u64());
    }

    GENERATED_ZOBRIST_EN_PASSANT_KEYS[SQ_NB] = (random_u64() & random_u64() & random_u64());

    GENERATED_ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_WHITE] = (random_u64() & random_u64() & random_u64());
    GENERATED_ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_BLACK] = (random_u64() & random_u64() & random_u64());

    for (size_t index = 0; index <= CASTLE_ANY; index++) {
        GENERATED_ZOBRIST_CASTLING_KEYS[index] = (random_u64() & random_u64() & random_u64());
    }

    // print_side_to_move_key();
    // print_castling_keys();
    // print_en_passant_keys();
    // print_piece_keys();
}

static void print_side_to_move_key(void) {
    printf("\nGENERATED_ZOBRIST_SIDE_TO_MOVE_KEY\n");
    printf("{ 0x%016" PRIx64 "ULL, 0x%016" PRIx64 "ULL }\n", GENERATED_ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_WHITE], GENERATED_ZOBRIST_SIDE_TO_MOVE_KEY[COLOR_BLACK]);
}

static void print_castling_keys(void) {
    printf("\nGENERATED_ZOBRIST_CASTLING_KEYS\n");
    printf("{\n\t");
    for (size_t index = 0; index <= CASTLE_ANY; index++) {
        printf("0x%016" PRIx64 "ULL", GENERATED_ZOBRIST_CASTLING_KEYS[index]);
        if (index % 4 == 3) {
            if (index < CASTLE_ANY) {
                printf(",\n\t");
            }
        } else {
            printf(", ");
        }
    }

    printf("\n}\n");
}

static void print_en_passant_keys(void) {
    printf("\nGENERATED_ZOBRIST_EN_PASSANT_KEYS\n");
    printf("{\n\t");
    for (Square sq = SQ_A1; sq <= SQ_NB; sq++) {
        printf("0x%016" PRIx64 "ULL", GENERATED_ZOBRIST_EN_PASSANT_KEYS[sq]);
        if (sq % 4 == 3) {
            if (sq <= SQ_H8) {
                printf(",\n\t");
            }
        } else {
            printf(", ");
        }
    }

    printf("\n}\n");
}

static void print_piece_keys(void) {
    printf("\nGENERATED_ZOBRIST_PIECE_KEYS\n");
    printf("{\n");
    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        printf("\t{\n\t\t");
        for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
            printf("0x%016" PRIx64 "ULL", GENERATED_ZOBRIST_PIECE_KEYS[piece][sq]);

            if (sq < SQ_H8) {
                printf(", ");
            }

            if (sq % 4 == 3) {
                if (sq < SQ_H8) {
                    printf("\n\t\t");
                } else {
                    printf("\n\t");
                }
            }
        }

        printf("}");
        if (piece < BLACK_KING) {
            printf(",\n");
        }
    }
    printf("\n}\n");
}
