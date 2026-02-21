#include <stdio.h>
#include <assert.h>
#include "attacks.h"
#include "bitboard.h"
#include "board.h"

static void test_get_bishop_attacks_from_d4_on_empty_board(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, d4);

    Bitboard bishop_attacks = get_bishop_attacks(d4, board_occupancy(&board));
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, c3);
    bb_set(&expected_attacks, b2);
    bb_set(&expected_attacks, a1);

    bb_set(&expected_attacks, e3);
    bb_set(&expected_attacks, f2);
    bb_set(&expected_attacks, g1);

    bb_set(&expected_attacks, c5);
    bb_set(&expected_attacks, b6);
    bb_set(&expected_attacks, a7);

    bb_set(&expected_attacks, e5);
    bb_set(&expected_attacks, f6);
    bb_set(&expected_attacks, g7);
    bb_set(&expected_attacks, h8);

    assert(bishop_attacks == expected_attacks);
}

static void test_get_bishop_attacks_from_d4_with_blocker_on_e5(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, d4);
    place_piece_on_sq(&board, WHITE_KNIGHT, e5);

    Bitboard bishop_attacks = get_bishop_attacks(d4, board_occupancy(&board));
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, c3);
    bb_set(&expected_attacks, b2);
    bb_set(&expected_attacks, a1);

    bb_set(&expected_attacks, e3);
    bb_set(&expected_attacks, f2);
    bb_set(&expected_attacks, g1);

    bb_set(&expected_attacks, c5);
    bb_set(&expected_attacks, b6);
    bb_set(&expected_attacks, a7);

    bb_set(&expected_attacks, e5);

    assert(bishop_attacks == expected_attacks);
}

static void test_get_bishop_attacks_from_d4_with_blocker_on_g7(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, d4);
    place_piece_on_sq(&board, WHITE_KNIGHT, g7);

    Bitboard bishop_attacks = get_bishop_attacks(d4, board_occupancy(&board));
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, c3);
    bb_set(&expected_attacks, b2);
    bb_set(&expected_attacks, a1);

    bb_set(&expected_attacks, e3);
    bb_set(&expected_attacks, f2);
    bb_set(&expected_attacks, g1);

    bb_set(&expected_attacks, c5);
    bb_set(&expected_attacks, b6);
    bb_set(&expected_attacks, a7);

    bb_set(&expected_attacks, e5);
    bb_set(&expected_attacks, f6);
    bb_set(&expected_attacks, g7);

    assert(bishop_attacks == expected_attacks);
}

static void test_get_bishop_attacks_from_d4_with_blockers_on_e5_and_f6(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, d4);
    place_piece_on_sq(&board, WHITE_KNIGHT, e5);
    place_piece_on_sq(&board, BLACK_ROOK, f6);

    Bitboard bishop_attacks = get_bishop_attacks(d4, board_occupancy(&board));
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, c3);
    bb_set(&expected_attacks, b2);
    bb_set(&expected_attacks, a1);

    bb_set(&expected_attacks, e3);
    bb_set(&expected_attacks, f2);
    bb_set(&expected_attacks, g1);

    bb_set(&expected_attacks, c5);
    bb_set(&expected_attacks, b6);
    bb_set(&expected_attacks, a7);

    bb_set(&expected_attacks, e5);

    assert(bishop_attacks == expected_attacks);
}

static void test_get_bishop_attacks_from_b2_with_blockers_outside_relevant_attacks(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_BISHOP, b2);

    place_piece_on_sq(&board, BLACK_ROOK, b3);

    Bitboard bishop_attacks = get_bishop_attacks(b2, board_occupancy(&board));
    Bitboard expected_attacks = 0ULL;
    bb_set(&expected_attacks, a1);
    bb_set(&expected_attacks, c1);
    bb_set(&expected_attacks, a3);
    bb_set(&expected_attacks, c3);
    bb_set(&expected_attacks, d4);
    bb_set(&expected_attacks, e5);
    bb_set(&expected_attacks, f6);
    bb_set(&expected_attacks, g7);
    bb_set(&expected_attacks, h8);
    
    assert(bishop_attacks == expected_attacks);
}

int main(void) {
    init_attack_tables();
    test_get_bishop_attacks_from_d4_on_empty_board();
    test_get_bishop_attacks_from_d4_with_blocker_on_e5();
    test_get_bishop_attacks_from_d4_with_blocker_on_g7();
    test_get_bishop_attacks_from_d4_with_blockers_on_e5_and_f6();
    test_get_bishop_attacks_from_b2_with_blockers_outside_relevant_attacks();

    printf("✅ All tests in attacks_test.c passed.\n");
    return 0;
}
