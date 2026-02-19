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

int main(void) {
    test_bb_test();
    test_bb_set_and_clear();
    printf("✅ All tests in bitboard_test.c passed.\n");
    return 0;
}

