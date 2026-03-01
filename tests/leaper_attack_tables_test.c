#include "bitboard.h"
#include "leaper_attack_tables.h"
#include "test_util.h"

static int test_knight_attacks(void) {
    const Bitboard b1_attacks = knight_attacks[SQ_B1];
    ASSERT_INT_EQ(3, bb_popcount(b1_attacks), "Knight on B1 should have 3 attacks");

    const Bitboard d2_mask = 1ULL << SQ_D2;
    ASSERT_UINT64_EQ(d2_mask, b1_attacks & d2_mask, "Knight on B1 must attack D2");

    const Bitboard gh_files = 0xC0C0C0C0C0C0C0C0ULL;
    ASSERT_UINT64_EQ(0ULL, b1_attacks & gh_files, "Knight on B1 should not wrap to G/H files");

    return 0;
}

static int test_king_attacks(void) {
    const Bitboard e4_attacks = king_attacks[SQ_E4];
    ASSERT_INT_EQ(8, bb_popcount(e4_attacks), "King on E4 should have 8 attacks");

    const Bitboard h1_attacks = king_attacks[SQ_H1];
    ASSERT_INT_EQ(3, bb_popcount(h1_attacks), "King on H1 should have 3 attacks");

    const Bitboard a_file = 0x0101010101010101ULL;
    ASSERT_UINT64_EQ(0ULL, h1_attacks & a_file, "King on H1 should not wrap to A-file");

    return 0;
}

static int test_pawn_attacks(void) {
    const Bitboard wp_a2 = pawn_attacks[COLOR_WHITE][SQ_A2];
    ASSERT_INT_EQ(1, bb_popcount(wp_a2), "White pawn on A2 should have 1 attack");
    ASSERT_UINT64_EQ(1ULL << SQ_B3, wp_a2, "White pawn on A2 must attack B3");

    const Bitboard wp_h2 = pawn_attacks[COLOR_WHITE][SQ_H2];
    const Bitboard a_file = 0x0101010101010101ULL;
    ASSERT_UINT64_EQ(0ULL, wp_h2 & a_file, "White pawn on H2 should not wrap to A-file");

    const Bitboard bp_d7 = pawn_attacks[COLOR_BLACK][SQ_D7];
    const Bitboard expected_b_d7 = (1ULL << SQ_C6) | (1ULL << SQ_E6);
    ASSERT_UINT64_EQ(expected_b_d7, bp_d7, "Black pawn on D7 should attack C6 and E6");

    return 0;
}

int main(void) {
    if (test_knight_attacks() != 0) return 1;
    if (test_king_attacks() != 0) return 1;
    if (test_pawn_attacks() != 0) return 1;

    printf("✅ All tests in leaper_attack_tables_test.c passed.\n");
    return 0;
}
