#include <assert.h>
#include <stdio.h>
#include "bitboard.h"
#include "board.h"

static void test_bb_test(void) {
    Bitboard bb = 0;
    assert(bb_test(bb, a1) == false);
    assert(bb_test(bb, h8) == false);

    bb_set(&bb, a1);
    assert(bb_test(bb, a1) == true);
    assert(bb_test(bb, h8) == false);

    bb_set(&bb, h8);
    assert(bb_test(bb, a1) == true);
    assert(bb_test(bb, h8) == true);

    bb_clear(&bb, a1);
    assert(bb_test(bb, a1) == false);
    assert(bb_test(bb, h8) == true);

    bb_clear(&bb, h8);
    assert(bb_test(bb, a1) == false);
    assert(bb_test(bb, h8) == false);
}

static void test_bb_set_and_clear(void) {
    Bitboard bb = 0;
    for (Square sq = 0; sq < SQ_NB; sq++) {
        bb_set(&bb, sq);
        assert(bb_test(bb, sq) == true);
    }

    for (Square sq = 0; sq < SQ_NB; sq++) {
        bb_clear(&bb, sq);
        assert(bb_test(bb, sq) == false);
    }
}

static void test_bb_popcount(void) {
    Bitboard bb = 0xFF0000ULL;
    int8_t popcount = bb_popcount(bb);
    assert(popcount == 8);

    bb = 0ULL;
    popcount = bb_popcount(bb);
    assert(popcount == 0);
}

static void test_bb_find_lssb_index(void) {
    Bitboard bb = 0xFF0000ULL;
    int8_t lssb_index = bb_find_lssb_index(bb);
    assert(lssb_index == 16);

    bb = 0ULL;
    lssb_index = bb_find_lssb_index(bb);
    assert(lssb_index == -1);
}

static void test_bb_pop_lssb(void) {
    Bitboard bb = 0xFF0000ULL;
    bb_pop_lssb(&bb);
    assert(bb == 0xFE0000ULL);

    bb = 0ULL;
    bb_pop_lssb(&bb);
    assert(bb == 0ULL);
}

int main(void) {
    test_bb_test();
    test_bb_set_and_clear();
    test_bb_popcount();
    test_bb_find_lssb_index();
    test_bb_pop_lssb();
    printf("✅ All tests in bitboard_test.c passed.\n");
    return 0;
}

