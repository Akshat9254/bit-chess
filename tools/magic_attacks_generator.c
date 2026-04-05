#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "bitboard.h"
#include "magic_attack_tables.h"
#include "square.h"

Magic generated_bishop_magics[SQ_NB];
Magic generated_rook_magics[SQ_NB];
Bitboard generated_sliding_pieces_attack_table[ATTACK_TABLE_SIZE];

U32 offset = 0;

static Bitboard generate_bishop_relevant_attacks_from_sq(Square sq);
static Bitboard generate_bishop_attacks_on_the_fly_from_sq(Square sq, Bitboard blockers);
static void init_bishop_attacks(void);
static void generate_bishop_attacks_from_sq(Square sq);

static Bitboard generate_rook_relevant_attacks_from_sq(Square sq);
static Bitboard generate_rook_attacks_on_the_fly_from_sq(Square sq, Bitboard blockers);
static void init_rook_attacks(void);
static void generate_rook_attacks_from_sq(Square sq);

static Bitboard set_occupancy(U32 index, U32 bits, Bitboard attack_mask);
static inline U64 random_u64(void);
static inline U64 random_magic(void);

static void print_magic(const Magic *magic);
void write_attack_table_to_file(const char *filename, const Bitboard *attack_table, size_t size);

int main(void) {
    offset = 0;
    srand(0); // NOLINT(*-msc51-cpp)

    init_bishop_attacks();
    init_rook_attacks();

    printf("Initialized Magic Attack Tables\n");
    printf("Uncomment the code in tools/magic_attacks_generator.c to print the attack tables\n");

    // printf("Bishop Magics: \n");
    // printf("{ \n");
    // for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
    //     printf("\t");
    //     print_magic(&generated_bishop_magics[sq]);
    //     if (sq < SQ_H8) {
    //         printf(", \n");
    //     } else {
    //         printf(" ");
    //     }
    // }
    // printf("\n}\n\n");

    // printf("Rook Magics: \n");
    // printf("{ \n");
    // for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
    //     printf("\t");
    //     print_magic(&generated_rook_magics[sq]);
    //     if (sq < SQ_H8) {
    //         printf(", \n");
    //     } else {
    //         printf(" ");
    //     }
    // }
    // printf("\n}\n\n");

    // write_attack_table_to_file("sliding_attack_table.tmp",
    //     generated_sliding_pieces_attack_table,
    //     ATTACK_TABLE_SIZE);
    // printf("Sliding Pieces Attack Table copied to file: sliding_attack_table.tmp\n");

    return 0;
}

static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq) {
    const Rank rank = rank_of_sq(sq);
    const File file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    Rank to_rank = rank - 1;
    File to_file = file - 1;
    for (; RANK_1 < to_rank && to_rank < RANK_8 && FILE_A < to_file && to_file < FILE_H; to_rank--, to_file--) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    to_rank = rank - 1;
    to_file = file + 1;
    for (; RANK_1 < to_rank && to_rank < RANK_8 && FILE_A < to_file && to_file < FILE_H; to_rank--, to_file++) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    to_rank = rank + 1;
    to_file = file - 1;
    for (; RANK_1 < to_rank && to_rank < RANK_8 && FILE_A < to_file && to_file < FILE_H; to_rank++, to_file--) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    to_rank = rank + 1;
    to_file = file + 1;
    for (; RANK_1 < to_rank && to_rank < RANK_8 && FILE_A < to_file && to_file < FILE_H; to_rank++, to_file++) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    return attack_mask;
}

static void init_bishop_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        generate_bishop_attacks_from_sq(sq);
    }
}

static void generate_bishop_attacks_from_sq(const Square sq) {
    const Bitboard relevant_attacks = generate_bishop_relevant_attacks_from_sq(sq);
    const U8 relevant_attack_bits = bb_popcount(relevant_attacks);
    const U32 relevant_count = (1 << relevant_attack_bits);

    Bitboard attacks_on_the_fly[MAX_BISHOP_OCC];
    Bitboard occ[MAX_BISHOP_OCC];

    for (U32 index = 0; index < relevant_count; index++) {
        occ[index] = set_occupancy(index, relevant_attack_bits, relevant_attacks);
        attacks_on_the_fly[index] = generate_bishop_attacks_on_the_fly_from_sq(sq, occ[index]);
    }

    Bitboard attacks[MAX_BISHOP_OCC];
    bool filled[MAX_BISHOP_OCC];
    bool found = false;
    U64 magic_no = 0ULL;
    const U8 shift = SQ_NB - relevant_attack_bits;

    while (!found) {
        const U64 magic = random_magic();
        bool fail = false;
        memset(attacks, 0, sizeof(attacks));
        memset(filled, false, sizeof(filled));

        for (U32 index = 0; index < relevant_count; index++) {
            const U32 occ_index = ((occ[index] * magic) >> shift);
            if (!filled[occ_index]) {
                attacks[occ_index] = attacks_on_the_fly[index];
                filled[occ_index] = true;
            } else if (attacks[occ_index] != attacks_on_the_fly[index]) {
                fail = true;
                break;
            }
        }

        if (!fail) {
            found = true;
            magic_no = magic;
        }
    }

    const Magic magic = {
        .relevant_attacks = relevant_attacks,
        .magic_no = magic_no,
        .attack_offset = offset,
        .shift = shift
    };

    for (U32 index = 0; index < relevant_count; index++) {
        const U32 occ_index = ((occ[index] * magic_no) >> shift);
        generated_sliding_pieces_attack_table[offset + occ_index] = attacks_on_the_fly[index];
    }

    offset += relevant_count;
    generated_bishop_magics[sq] = magic;
}

static Bitboard generate_bishop_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers) {
    const Rank rank = rank_of_sq(sq);
    const File file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    Rank to_rank = rank - 1;
    File to_file = file - 1;
    for (; to_rank <= RANK_8 && to_file <= FILE_H; to_rank--, to_file--) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    to_rank = rank - 1;
    to_file = file + 1;
    for (; to_rank <= RANK_8 && to_file <= FILE_H; to_rank--, to_file++) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    to_rank = rank + 1;
    to_file = file - 1;
    for (; to_rank <= RANK_8 && to_file <= FILE_H; to_rank++, to_file--) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    to_rank = rank + 1;
    to_file = file + 1;
    for (; to_rank <= RANK_8 && to_file <= FILE_H; to_rank++, to_file++) {
        const Square to = rank_file_to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    return attack_mask;
}

static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq) {
    const Rank rank = rank_of_sq(sq);
    const File file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (Rank to_rank = rank - 1; RANK_1 < to_rank && to_rank < RANK_8; to_rank--) {
        const Square to = rank_file_to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (File to_file = file - 1; FILE_A < to_file && to_file < FILE_H; to_file--) {
        const Square to = rank_file_to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (Rank to_rank = rank + 1; RANK_1 < to_rank && to_rank < RANK_8; to_rank++) {
        const Square to = rank_file_to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (File to_file = file + 1; FILE_A < to_file && to_file < FILE_H; to_file++) {
        const Square to = rank_file_to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    return attack_mask;
}

static Bitboard generate_rook_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers) {
    const Rank rank = rank_of_sq(sq);
    const File file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (Rank to_rank = rank - 1; to_rank <= RANK_8; to_rank--) {
        const Square to = rank_file_to_sq(to_rank, file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (File to_file = file - 1; to_file <= FILE_H; to_file--) {
        const Square to = rank_file_to_sq(rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (Rank to_rank = rank + 1; to_rank <= RANK_8; to_rank++) {
        const Square to = rank_file_to_sq(to_rank, file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (File to_file = file + 1; to_file <= FILE_H; to_file++) {
        const Square to = rank_file_to_sq(rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    return attack_mask;
}

static void init_rook_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        generate_rook_attacks_from_sq(sq);
    }
}

static void generate_rook_attacks_from_sq(const Square sq) {
    const Bitboard relevant_attacks = generate_rook_relevant_attacks_from_sq(sq);
    const U8 relevant_attack_bits = bb_popcount(relevant_attacks);
    const U32 relevant_count = (1 << relevant_attack_bits);

    Bitboard attacks_on_the_fly[MAX_ROOK_OCC];
    Bitboard occ[MAX_ROOK_OCC];

    for (U32 index = 0; index < relevant_count; index++) {
        occ[index] = set_occupancy(index, relevant_attack_bits, relevant_attacks);
        attacks_on_the_fly[index] = generate_rook_attacks_on_the_fly_from_sq(sq, occ[index]);
    }

    Bitboard attacks[MAX_ROOK_OCC];
    bool filled[MAX_ROOK_OCC];
    bool found = false;
    U64 magic_no = 0ULL;
    const U8 shift = SQ_NB - relevant_attack_bits;

    while (!found) {
        const U64 magic = random_magic();
        bool fail = false;
        memset(attacks, 0, sizeof(attacks));
        memset(filled, false, sizeof(filled));

        for (uint32_t index = 0; index < relevant_count; index++) {
            uint32_t occ_index = ((occ[index] * magic) >> shift);
            if (!filled[occ_index]) {
                attacks[occ_index] = attacks_on_the_fly[index];
                filled[occ_index] = true;
            } else if (attacks[occ_index] != attacks_on_the_fly[index]) {
                fail = true;
                break;
            }
        }

        if (!fail) {
            found = true;
            magic_no = magic;
        }
    }

    const Magic magic = {
        .relevant_attacks = relevant_attacks,
        .magic_no = magic_no,
        .attack_offset = offset,
        .shift = shift
    };

    for (U32 index = 0; index < relevant_count; index++) {
        const U32 occ_index = ((occ[index] * magic_no) >> shift);
        generated_sliding_pieces_attack_table[offset + occ_index] = attacks_on_the_fly[index];
    }

    offset += relevant_count;
    generated_rook_magics[sq] = magic;
}

static Bitboard set_occupancy(const U32 index, const U32 bits, Bitboard attack_mask) {
    Bitboard occ = 0ULL;

    for (U32 i = 0; i < bits; i++) {
        if (index & (1 << i)) {
            const Square sq = bb_find_lssb_index(attack_mask);
            bb_set(&occ, sq);
        }

        bb_pop_lssb(&attack_mask);
    }

    return occ;
}

static inline U64 random_u64(void) {
    return ((U64) rand() & 0xFFFF) | // NOLINT(*-msc50-cpp)
        ((U64) rand() & 0xFFFF) << 16 | // NOLINT(*-msc50-cpp)
        ((U64) rand() & 0xFFFF) << 32 | // NOLINT(*-msc50-cpp)
        ((U64) rand() & 0xFFFF) << 48; // NOLINT(*-msc50-cpp)
}

static inline U64 random_magic(void) {
    return (random_u64() & random_u64() & random_u64());
}

static void print_magic(const Magic *magic) {
    printf("{ 0x%016" PRIx64 "ULL, 0x%016" PRIx64 "ULL, %" PRIu32 ", %" PRIu8 " }",
        magic->relevant_attacks,
        magic->magic_no,
        magic->attack_offset,
        magic->shift);
}

void write_attack_table_to_file(const char *filename, const Bitboard *attack_table, const size_t size) {
    printf("size = %zu\n", size);
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(f, "{\n");
    for (U32 index = 0; index < size; index++) {
        fprintf(f, "\t0x%016" PRIx64 "ULL", attack_table[index]);

        if (index < size - 1) {
            fprintf(f, ", ");
        } else {
            fprintf(f, "  ");
        }

        if (index % 4 == 3 || index == size - 1) {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "};\n");

    fclose(f);
}
