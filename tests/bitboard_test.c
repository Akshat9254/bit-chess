#include "bitboard.h"
#include "test_util.h"

static int test_bb_test(void) {
    Bitboard bb = 0ULL;

    // Initial state
    ASSERT_INT_EQ(false, bb_test(bb, SQ_A1), bb_test_empty_A1);
    ASSERT_INT_EQ(false, bb_test(bb, SQ_H8), bb_test_empty_H8);

    // Set and check
    bb_set(&bb, SQ_A1);
    ASSERT_INT_EQ(true,  bb_test(bb, SQ_A1), bb_test_set_A1);
    ASSERT_INT_EQ(false, bb_test(bb, SQ_H8), bb_test_still_empty_H8);

    bb_set(&bb, SQ_H8);
    ASSERT_INT_EQ(true,  bb_test(bb, SQ_A1), bb_test_still_set_A1);
    ASSERT_INT_EQ(true,  bb_test(bb, SQ_H8), bb_test_set_H8);

    // Clear and check
    bb_clear(&bb, SQ_A1);
    ASSERT_INT_EQ(false, bb_test(bb, SQ_A1), bb_test_clear_A1);
    ASSERT_INT_EQ(true,  bb_test(bb, SQ_H8), bb_test_remain_H8);

    bb_clear(&bb, SQ_H8);
    ASSERT_INT_EQ(false, bb_test(bb, SQ_A1), bb_test_final_empty_A1);
    ASSERT_INT_EQ(false, bb_test(bb, SQ_H8), bb_test_final_empty_H8);

    return 0;
}

static int test_bb_set_and_clear(void) {
    Bitboard bb = 0ULL;

    // Fill the board bit by bit
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        bb_set(&bb, sq);
        ASSERT_INT_EQ(true, bb_test(bb, sq), bb_set_loop);
    }

    // Clear the board bit by bit
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        bb_clear(&bb, sq);
        ASSERT_INT_EQ(false, bb_test(bb, sq), bb_clear_loop);
    }

    return 0;
}

static int test_bb_popcount(void) {
    Bitboard bb = 0xFF0000ULL; // Rank 3 full
    ASSERT_INT_EQ(8, bb_popcount(bb), bb_popcount_rank3);

    bb = 0ULL;
    ASSERT_INT_EQ(0, bb_popcount(bb), bb_popcount_empty);

    return 0;
}

static int test_bb_find_lssb_index(void) {
    Bitboard bb = 0xFF0000ULL; // LSSB is at SQ_A3 (index 16)
    ASSERT_INT_EQ(16, bb_find_lssb_index(bb), bb_find_lssb_index_full);

    bb = 0ULL;
    ASSERT_INT_EQ(-1, bb_find_lssb_index(bb), bb_find_lssb_index_empty);

    return 0;
}

static int test_bb_pop_lssb(void) {
    Bitboard bb = 0xFF0000ULL;
    bb_pop_lssb(&bb);
    // 0xFF0000 is 11111111 00000000 00000000. Popping LSSB leaves 11111110...
    ASSERT_UINT64_EQ(0xFE0000ULL, bb, bb_pop_lssb_standard);

    bb = 0ULL;
    bb_pop_lssb(&bb);
    ASSERT_UINT64_EQ(0ULL, bb, bb_pop_lssb_empty);

    return 0;
}

int main(void) {
    if (test_bb_test() != 0) return 1;
    if (test_bb_set_and_clear() != 0) return 1;
    if (test_bb_popcount() != 0) return 1;
    if (test_bb_find_lssb_index() != 0) return 1;
    if (test_bb_pop_lssb() != 0) return 1;

    printf("✅ All tests in bitboard_test.c passed.\n");
    return 0;
}
