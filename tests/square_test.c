#include "square.h"
#include "test_util.h"

static int test_boundaries(void) {
    ASSERT_INT_EQ(true,  is_sq_on_board(SQ_A1), is_sq_on_board);
    ASSERT_INT_EQ(true,  is_sq_on_board(SQ_H8), is_sq_on_board);
    ASSERT_INT_EQ(false, is_sq_on_board(-1), is_sq_on_board_under);
    ASSERT_INT_EQ(false, is_sq_on_board(64), is_sq_on_board_over);

    ASSERT_INT_EQ(true,  is_rank_on_board(RANK_1), is_rank_on_board);
    ASSERT_INT_EQ(true,  is_file_on_board(FILE_H), is_file_on_board);
    ASSERT_INT_EQ(false, is_rank_on_board(8), is_rank_on_board_over);
    ASSERT_INT_EQ(false, is_file_on_board(-1), is_file_on_board_under);

    return 0;
}

static int test_extraction(void) {
    ASSERT_INT_EQ(RANK_1, rank_of_sq(SQ_A1), rank_of_sq);
    ASSERT_INT_EQ(FILE_A, file_of_sq(SQ_A1), file_of_sq);

    ASSERT_INT_EQ(RANK_1, rank_of_sq(SQ_B1), rank_of_sq);
    ASSERT_INT_EQ(FILE_B, file_of_sq(SQ_B1), file_of_sq);

    ASSERT_INT_EQ(RANK_2, rank_of_sq(SQ_A2), rank_of_sq);
    ASSERT_INT_EQ(FILE_A, file_of_sq(SQ_A2), file_of_sq);

    ASSERT_INT_EQ(RANK_8, rank_of_sq(SQ_H8), rank_of_sq);
    ASSERT_INT_EQ(FILE_H, file_of_sq(SQ_H8), file_of_sq);

    return 0;
}

static int test_reconstruction(void) {
    ASSERT_INT_EQ(SQ_A1, rank_file_to_sq(RANK_1, FILE_A), rank_file_to_sq);
    ASSERT_INT_EQ(SQ_E4, rank_file_to_sq(RANK_4, FILE_E), rank_file_to_sq);
    ASSERT_INT_EQ(SQ_H8, rank_file_to_sq(RANK_8, FILE_H), rank_file_to_sq);

    return 0;
}

static int test_integrity_loop(void) {
    for (int i = SQ_A1; i <= SQ_H8; i++) {
        const Square original = (Square) i;
        const Rank r = rank_of_sq(original);
        const File f = file_of_sq(original);
        const Square reconstructed = rank_file_to_sq(r, f);

        ASSERT_INT_EQ(original, reconstructed, square_integrity_loop);
    }
    return 0;
}

int main(void) {
    if (test_boundaries() != 0)     return 1;
    if (test_extraction() != 0)     return 1;
    if (test_reconstruction() != 0) return 1;
    if (test_integrity_loop() != 0) return 1;

    printf("✅ All tests in square_test.c passed.\n");
    return 0;
}
