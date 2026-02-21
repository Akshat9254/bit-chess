#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "square.h"
#include "attacks.h"
#include "board.h"

Magic bishop_magics[SQ_NB];
Magic rook_magics[SQ_NB];
Bitboard sliding_pieces_attack_table[ATTACK_TABLE_SIZE];

bool is_initialized = true;
U32 offset = 0;

static Bitboard set_occupancy(uint32_t index, uint32_t bits, Bitboard attack_mask);
static inline U64 random_u64(void);
static inline U64 random_magic(void);

static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq);
static Bitboard generate_bishop_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers);
static void init_bishop_attacks(void);
static void generate_bishop_attacks_from_sq(const Square sq);

static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq);
static Bitboard generate_rook_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers);
static void init_rook_attacks(void);
static void generate_rook_attacks_from_sq(const Square sq);

int main(void) {
    offset = 0;
    srand(0);

    init_bishop_attacks();
    init_rook_attacks();

    return 0;
}

static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1, to_file = file - 1; to_rank > RANK_1 && to_file > FILE_A; to_rank--, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank - 1, to_file = file + 1; to_rank > RANK_1 && to_file < FILE_H; to_rank--, to_file++) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1, to_file = file - 1; to_rank < RANK_8 && to_file > FILE_A; to_rank++, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1, to_file = file + 1; to_rank < RANK_8 && to_file < FILE_H; to_rank++, to_file++) {
        Square to = to_sq(to_rank, to_file);
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
    Bitboard relevant_attacks = generate_bishop_relevant_attacks_from_sq(sq);
    uint8_t relevant_attack_bits = bb_popcount(relevant_attacks);
    uint32_t relevant_count = (1 << relevant_attack_bits);
    
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
    U8 shift = SQ_NB - relevant_attack_bits;

    while (!found) {
        U64 magic = random_magic();
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

    Magic magic = {
        .relevant_attacks = relevant_attacks,
        .magic_no = magic_no,
        .attack_offset = offset,
        .shift = shift
    };
    
    for (U32 index = 0; index < relevant_count; index++) {
        uint32_t occ_index = ((occ[index] * magic_no) >> shift);
        sliding_pieces_attack_table[offset + occ_index] = attacks_on_the_fly[index];
    }

    offset += relevant_count;
    bishop_magics[sq] = magic;
}

static Bitboard generate_bishop_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1, to_file = file - 1; to_rank >= RANK_1 && to_file >= FILE_A; to_rank--, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_rank = rank - 1, to_file = file + 1; to_rank >= RANK_1 && to_file <= FILE_H; to_rank--, to_file++) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_rank = rank + 1, to_file = file - 1; to_rank <= RANK_8 && to_file >= FILE_A; to_rank++, to_file--) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_rank = rank + 1, to_file = file + 1; to_rank <= RANK_8 && to_file <= FILE_H; to_rank++, to_file++) {
        Square to = to_sq(to_rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    return attack_mask;
}

static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1; to_rank > RANK_1; to_rank--) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_file = file - 1; to_file > FILE_A; to_file--) {
        Square to = to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_rank = rank + 1; to_rank < RANK_8; to_rank++) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
    }

    for (int8_t to_file = file + 1; to_file < FILE_H; to_file++) {
        Square to = to_sq(rank, to_file);
        bb_set(&attack_mask, to);
    }

    return attack_mask;
}

static Bitboard generate_rook_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers) {
    const int8_t rank = rank_of_sq(sq);
    const int8_t file = file_of_sq(sq);
    Bitboard attack_mask = 0ULL;

    for (int8_t to_rank = rank - 1; to_rank >= RANK_1; to_rank--) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_file = file - 1; to_file >= FILE_A; to_file--) {
        Square to = to_sq(rank, to_file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_rank = rank + 1; to_rank <= RANK_8; to_rank++) {
        Square to = to_sq(to_rank, file);
        bb_set(&attack_mask, to);
        if (bb_test(blockers, to)) {
            break;
        }
    }

    for (int8_t to_file = file + 1; to_file <= FILE_H; to_file++) {
        Square to = to_sq(rank, to_file);
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
    Bitboard relevant_attacks = generate_rook_relevant_attacks_from_sq(sq);
    uint8_t relevant_attack_bits = bb_popcount(relevant_attacks);
    uint32_t relevant_count = (1 << relevant_attack_bits);
    
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
    U8 shift = SQ_NB - relevant_attack_bits;

    while (!found) {
        U64 magic = random_magic();
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

    Magic magic = {
        .relevant_attacks = relevant_attacks,
        .magic_no = magic_no,
        .attack_offset = offset,
        .shift = shift
    };

    for (U32 index = 0; index < relevant_count; index++) {
        uint32_t occ_index = ((occ[index] * magic_no) >> shift);
        sliding_pieces_attack_table[offset + occ_index] = attacks_on_the_fly[index];
    }

    offset += relevant_count;
    rook_magics[sq] = magic;
}

static Bitboard set_occupancy(uint32_t index, uint32_t bits, Bitboard attack_mask) {
    Bitboard occ = 0ULL;

    for (uint32_t i = 0; i < bits; i++) {
        if (index & (1 << i)) {
            Square sq = bb_find_lssb_index(attack_mask);
            assert(sq != -1);
            bb_set(&occ, sq);
        }

        bb_pop_lssb(&attack_mask);
    }

    return occ;
}

static inline U64 random_u64(void) {
    return ((U64) rand() & 0xFFFF) |
        ((U64) rand() & 0xFFFF) << 16 |
        ((U64) rand() & 0xFFFF) << 32 |
        ((U64) rand() & 0xFFFF) << 48;
}

static inline U64 random_magic(void) {
    return (random_u64() & random_u64() & random_u64());
}
