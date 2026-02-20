#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "square.h"
#include "attacks.h"
#include "board.h"

bool is_initialized = false;

Bitboard knight_attacks[SQ_NB];
Bitboard king_attacks[SQ_NB];
Bitboard pawn_attacks[COLOR_NB][SQ_NB];

Bitboard bishop_relevant_attacks[SQ_NB];
U64 bishop_magics[SQ_NB];
U8 bishop_shift[SQ_NB];
Bitboard bishop_attacks[SQ_NB][MAX_BISHOP_OCC];

Bitboard rook_relevant_attacks[SQ_NB];
U64 rook_magics[SQ_NB];
U8 rook_shift[SQ_NB];
Bitboard rook_attacks[SQ_NB][MAX_ROOK_OCC];

static void init_knight_attack_table(void);
static void init_king_attack_table(void);
static void init_pawn_attack_table(void);
static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size);

static Bitboard set_occupancy(uint32_t index, uint32_t bits, Bitboard attack_mask);
static inline U64 random_u64(void);
static inline U64 random_magic(void);

static void init_bishop_relevant_attacks(void);
static Bitboard generate_bishop_relevant_attacks_from_sq(const Square sq);
static Bitboard generate_bishop_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers);
static void init_bishop_attacks(void);
static void generate_bishop_attacks_from_sq(const Square sq);

static void init_rook_relevant_attacks(void);
static Bitboard generate_rook_relevant_attacks_from_sq(const Square sq);
static Bitboard generate_rook_attacks_on_the_fly_from_sq(const Square sq, const Bitboard blockers);
static void init_rook_attacks(void);
static void generate_rook_attacks_from_sq(const Square sq);

void init_attack_tables(void) {
    if (is_initialized) {
        return;
    }

    srand(time(NULL));
    init_knight_attack_table();
    init_king_attack_table();
    init_bishop_relevant_attacks();
    init_bishop_attacks();
    init_rook_relevant_attacks();
    init_rook_attacks();
    is_initialized = true;
}

static void init_knight_attack_table(void) {
    const Offset offsets[8] = {
        { -2, -1 }, { -1, -2 }, { 1, -2 }, { 2, -1 }, 
        { 2, 1 }, { 1, 2 }, { -1, 2 }, { -2, 1 }
    };

    init_leaper_attacks(knight_attacks, offsets, 8);
}

static void init_king_attack_table(void) {
    const Offset offsets[8] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
        { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }
    };

    init_leaper_attacks(king_attacks, offsets, 8);
}

static void init_pawn_attack_table(void) {
    const Offset white_offsets[2] = {{ 1, -1 }, { 1, 1 }};
    const Offset black_offsets[2] = {{ -1, -1 }, { -1, 1 }};

    init_leaper_attacks(pawn_attacks[WHITE], white_offsets, 2);
    init_leaper_attacks(pawn_attacks[BLACK], black_offsets, 2);
}

static void init_leaper_attacks(Bitboard attack_table[SQ_NB], const Offset offsets[], size_t offset_size) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        Bitboard attack_mask = 0ULL;
        const uint8_t rank = rank_of_sq(sq);
        const uint8_t file = file_of_sq(sq);

        for (uint8_t i = 0; i < offset_size; i++) {
            int8_t to_rank = rank + offsets[i].delta_rank;
            int8_t to_file = file + offsets[i].delta_file;

            if (is_rank_file_on_board(to_rank, to_file)) {
                bb_set(&attack_mask, to_sq(to_rank, to_file));
            }
        }

        attack_table[sq] = attack_mask;
    }
}

static void init_bishop_relevant_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        bishop_relevant_attacks[sq] = generate_bishop_relevant_attacks_from_sq(sq);
    }
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
    Bitboard relevant_attacks = bishop_relevant_attacks[sq];
    uint8_t relevant_attack_bits = bb_popcount(relevant_attacks);
    uint32_t relevant_count = (1 << relevant_attack_bits);
    
    bishop_shift[sq] = SQ_NB - relevant_attack_bits;
    
    Bitboard attacks_on_the_fly[MAX_BISHOP_OCC];
    Bitboard occ[MAX_BISHOP_OCC];

    for (u_int32_t index = 0; index < relevant_count; index++) {
        occ[index] = set_occupancy(index, relevant_attack_bits, relevant_attacks);
        attacks_on_the_fly[index] = generate_bishop_attacks_on_the_fly_from_sq(sq, occ[index]);
    }
    
    Bitboard attacks[MAX_BISHOP_OCC];
    bool filled[MAX_BISHOP_OCC];
    bool found = false;

    while (!found) {
        U64 magic = random_magic();
        bool fail = false;
        memset(attacks, 0, sizeof(attacks));
        memset(filled, false, sizeof(filled));

        for (uint32_t index = 0; index < relevant_count; index++) {
            uint32_t occ_index = ((occ[index] * magic) >> bishop_shift[sq]);
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
            bishop_magics[sq] = magic;
        }
    }

    for (u_int32_t index = 0; index < relevant_count; index++) {
        uint32_t occ_index = ((occ[index] * bishop_magics[sq]) >> bishop_shift[sq]);
        bishop_attacks[sq][occ_index] = attacks_on_the_fly[index];
    }
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

static void init_rook_relevant_attacks(void) {
    for (Square sq = 0; sq < SQ_NB; sq++) {
        rook_relevant_attacks[sq] = generate_rook_relevant_attacks_from_sq(sq);
    }
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
    Bitboard relevant_attacks = rook_relevant_attacks[sq];
    uint8_t relevant_attack_bits = bb_popcount(relevant_attacks);
    uint32_t relevant_count = (1 << relevant_attack_bits);
    
    rook_shift[sq] = SQ_NB - relevant_attack_bits;
    
    Bitboard attacks_on_the_fly[MAX_ROOK_OCC];
    Bitboard occ[MAX_ROOK_OCC];

    for (u_int32_t index = 0; index < relevant_count; index++) {
        occ[index] = set_occupancy(index, relevant_attack_bits, relevant_attacks);
        attacks_on_the_fly[index] = generate_rook_attacks_on_the_fly_from_sq(sq, occ[index]);
    }
    
    Bitboard attacks[MAX_ROOK_OCC];
    bool filled[MAX_ROOK_OCC];
    bool found = false;

    while (!found) {
        U64 magic = random_magic();
        bool fail = false;
        memset(attacks, 0, sizeof(attacks));
        memset(filled, false, sizeof(filled));

        for (uint32_t index = 0; index < relevant_count; index++) {
            uint32_t occ_index = ((occ[index] * magic) >> rook_shift[sq]);
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
            rook_magics[sq] = magic;
        }
    }

    for (u_int32_t index = 0; index < relevant_count; index++) {
        uint32_t occ_index = ((occ[index] * rook_magics[sq]) >> rook_shift[sq]);
        rook_attacks[sq][occ_index] = attacks_on_the_fly[index];
    }
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
