#include "board.h"
#include "magic_attack_tables.h"
#include "test_util.h"

static int test_get_bishop_attacks_from_d4_on_empty_board(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, SQ_D4);

    const Bitboard bishop_attacks = get_bishop_attacks(SQ_D4, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;

    // Diagonals from SQ_D4
    bb_set(&expected_attacks, SQ_C3); bb_set(&expected_attacks, SQ_B2); bb_set(&expected_attacks, SQ_A1);
    bb_set(&expected_attacks, SQ_E3); bb_set(&expected_attacks, SQ_F2); bb_set(&expected_attacks, SQ_G1);
    bb_set(&expected_attacks, SQ_C5); bb_set(&expected_attacks, SQ_B6); bb_set(&expected_attacks, SQ_A7);
    bb_set(&expected_attacks, SQ_E5); bb_set(&expected_attacks, SQ_F6); bb_set(&expected_attacks, SQ_G7); bb_set(&expected_attacks, SQ_H8);

    ASSERT_UINT64_EQ(expected_attacks, bishop_attacks, test_get_bishop_attacks_from_d4_on_empty_board);
    return 0;
}

static int test_get_bishop_attacks_from_d4_with_blocker_on_e5(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_BISHOP, SQ_D4);
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_E5);

    Bitboard bishop_attacks = get_bishop_attacks(SQ_D4, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, SQ_C3); bb_set(&expected_attacks, SQ_B2); bb_set(&expected_attacks, SQ_A1);
    bb_set(&expected_attacks, SQ_E3); bb_set(&expected_attacks, SQ_F2); bb_set(&expected_attacks, SQ_G1);
    bb_set(&expected_attacks, SQ_C5); bb_set(&expected_attacks, SQ_B6); bb_set(&expected_attacks, SQ_A7);
    bb_set(&expected_attacks, SQ_E5); // Blocker is included in attacks

    ASSERT_UINT64_EQ(expected_attacks, bishop_attacks, test_get_bishop_attacks_from_SQ_D4_with_blocker_on_e5);
    return 0;
}

static int test_get_bishop_attacks_from_b2_with_blockers_outside_relevant_attacks(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_BISHOP, SQ_B2);
    // This blocker is on the same file, not the same diagonal
    place_piece_on_sq(&board, BLACK_ROOK, SQ_B3);

    Bitboard bishop_attacks = get_bishop_attacks(SQ_B2, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;
    bb_set(&expected_attacks, SQ_A1); bb_set(&expected_attacks, SQ_C1);
    bb_set(&expected_attacks, SQ_A3); bb_set(&expected_attacks, SQ_C3);
    bb_set(&expected_attacks, SQ_D4); bb_set(&expected_attacks, SQ_E5);
    bb_set(&expected_attacks, SQ_F6); bb_set(&expected_attacks, SQ_G7); bb_set(&expected_attacks, SQ_H8);

    ASSERT_UINT64_EQ(expected_attacks, bishop_attacks, test_get_bishop_attacks_from_b2_with_blockers_outside_relevant_attacks);
    return 0;
}

static int test_get_rook_attacks_from_d4_on_empty_board(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_ROOK, SQ_D4);

    Bitboard rook_attacks = get_rook_attacks(SQ_D4, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;

    // Rank 4 and File D
    bb_set(&expected_attacks, SQ_D5); bb_set(&expected_attacks, SQ_D6); bb_set(&expected_attacks, SQ_D7); bb_set(&expected_attacks, SQ_D8);
    bb_set(&expected_attacks, SQ_D3); bb_set(&expected_attacks, SQ_D2); bb_set(&expected_attacks, SQ_D1);
    bb_set(&expected_attacks, SQ_E4); bb_set(&expected_attacks, SQ_F4); bb_set(&expected_attacks, SQ_G4); bb_set(&expected_attacks, SQ_H4);
    bb_set(&expected_attacks, SQ_C4); bb_set(&expected_attacks, SQ_B4); bb_set(&expected_attacks, SQ_A4);

    ASSERT_UINT64_EQ(expected_attacks, rook_attacks, test_get_rook_attacks_from_d4_on_empty_board);
    return 0;
}

static int test_get_rook_attacks_from_d4_with_blocker_on_d6(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, BLACK_ROOK, SQ_D4);
    place_piece_on_sq(&board, WHITE_KNIGHT, SQ_D6);

    Bitboard rook_attacks = get_rook_attacks(SQ_D4, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, SQ_D5);
    bb_set(&expected_attacks, SQ_D6); // Blocked here
    bb_set(&expected_attacks, SQ_D3); bb_set(&expected_attacks, SQ_D2); bb_set(&expected_attacks, SQ_D1);
    bb_set(&expected_attacks, SQ_E4); bb_set(&expected_attacks, SQ_F4); bb_set(&expected_attacks, SQ_G4); bb_set(&expected_attacks, SQ_H4);
    bb_set(&expected_attacks, SQ_C4); bb_set(&expected_attacks, SQ_B4); bb_set(&expected_attacks, SQ_A4);

    ASSERT_UINT64_EQ(expected_attacks, rook_attacks, test_get_rook_attacks_from_d4_with_blocker_on_d6);
    return 0;
}

static int test_get_rook_attacks_from_b2_with_irrelevant_blocker(void) {
    Board board;
    clear_board(&board);
    place_piece_on_sq(&board, WHITE_ROOK, SQ_B2);
    // Blocker is diagonal to the rook (c3), so it should not affect rook attacks
    place_piece_on_sq(&board, BLACK_BISHOP, SQ_C3);

    Bitboard rook_attacks = get_rook_attacks(SQ_B2, board.occupied[COLOR_BOTH]);
    Bitboard expected_attacks = 0ULL;

    bb_set(&expected_attacks, SQ_B1); bb_set(&expected_attacks, SQ_B3); bb_set(&expected_attacks, SQ_B4);
    bb_set(&expected_attacks, SQ_B5); bb_set(&expected_attacks, SQ_B6); bb_set(&expected_attacks, SQ_B7); bb_set(&expected_attacks, SQ_B8);
    bb_set(&expected_attacks, SQ_A2); bb_set(&expected_attacks, SQ_C2); bb_set(&expected_attacks, SQ_D2);
    bb_set(&expected_attacks, SQ_E2); bb_set(&expected_attacks, SQ_F2); bb_set(&expected_attacks, SQ_G2); bb_set(&expected_attacks, SQ_H2);

    ASSERT_UINT64_EQ(expected_attacks, rook_attacks, test_get_rook_attacks_from_b2_with_irrelevant_blocker);
    return 0;
}

int main(void) {
    if (test_get_bishop_attacks_from_d4_on_empty_board() != 0) return 1;
    if (test_get_bishop_attacks_from_d4_with_blocker_on_e5() != 0) return 1;
    if (test_get_bishop_attacks_from_b2_with_blockers_outside_relevant_attacks() != 0) return 1;

    if (test_get_rook_attacks_from_d4_on_empty_board() != 0) return 1;
    if (test_get_rook_attacks_from_d4_with_blocker_on_d6() != 0) return 1;
    if (test_get_rook_attacks_from_b2_with_irrelevant_blocker() != 0) return 1;

    printf("✅ All tests in magic_attack_tables_test.c passed.\n");
    return 0;
}
