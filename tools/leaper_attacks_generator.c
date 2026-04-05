#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"

Bitboard generated_knight_attacks[SQ_NB];
Bitboard generated_king_attacks[SQ_NB];
Bitboard generated_pawn_attacks[COLOR_NB][SQ_NB];

typedef struct {
    int8_t delta_rank;
    int8_t delta_file;
} Offset;

static void init_knight_attack_table(void);
static void init_king_attack_table(void);
static void init_pawn_attack_table(void);
static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size);
static void print_attack_table(const Bitboard attack_table[SQ_NB]);

int main(void) {
    init_knight_attack_table();
    init_king_attack_table();
    init_pawn_attack_table();

    printf("Initialized Leaper Attack Tables\n");
    printf("Uncomment the code in tools/leaper_attacks_generator.c to print the attack tables\n");

    // printf("knight attacks: \n");
    // print_attack_table(generated_knight_attacks);
    // printf("\n\n");
    //
    // printf("king attacks: \n");
    // print_attack_table(generated_king_attacks);
    // printf("\n\n");
    //
    // printf("pawn attacks: \n");
    // printf("{ ");
    // print_attack_table(generated_pawn_attacks[COLOR_WHITE]);
    // printf(", \n");
    // print_attack_table(generated_pawn_attacks[COLOR_BLACK]);
    // printf(" }");
    // printf("\n\n");

    return 0;
}

static void init_knight_attack_table(void) {
    const Offset offsets[8] = {
        { -2, -1 }, { -1, -2 },
        { 1, -2 }, { 2, -1 },
        { 2, 1 }, { 1, 2 },
        { -1, 2 }, { -2, 1 }
    };

    init_leaper_attacks(generated_knight_attacks, offsets, 8);
}

static void init_king_attack_table(void) {
    const Offset offsets[8] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
        { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }
    };

    init_leaper_attacks(generated_king_attacks, offsets, 8);
}

static void init_pawn_attack_table(void) {
    const Offset white_offsets[2] = {{ 1, -1 }, { 1, 1 }};
    const Offset black_offsets[2] = {{ -1, -1 }, { -1, 1 }};

    init_leaper_attacks(generated_pawn_attacks[COLOR_WHITE], white_offsets, 2);
    init_leaper_attacks(generated_pawn_attacks[COLOR_BLACK], black_offsets, 2);
}

static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], const size_t offset_size) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        Bitboard attack_mask = 0ULL;
        const Rank rank = rank_of_sq(sq);
        const File file = file_of_sq(sq);

        for (size_t i = 0; i < offset_size; i++) {
            const int to_rank = rank + offsets[i].delta_rank;
            const int to_file = file + offsets[i].delta_file;

            if (is_rank_file_on_board(to_rank, to_file)) {
                bb_set(&attack_mask, rank_file_to_sq(to_rank, to_file));
            }
        }

        attack_table[sq] = attack_mask;
    }
}

static void print_attack_table(const Bitboard attack_table[SQ_NB]) {
    printf("{\n");
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        printf("\t0x%016" PRIx64 "ULL", attack_table[sq]);
        if (sq < SQ_H8) {
            printf(", ");
        } else {
            printf(" ");
        }

        if (sq % 4 == 3) {
            printf("\n");
        }
    }

    printf("}");
}
