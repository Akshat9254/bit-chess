#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_4(void) {
    for (U8 depth = 1; depth <= 6; depth++) {
        printf("[RUN] test_perft_pos_4 for depth: %d...\n", depth);

        const char *fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
        const U64 pert_result[] = {0ULL, 6ULL, 264ULL, 9467ULL, 422333ULL, 15833292ULL, 706045033ULL};

        Board board;
        FenError err;
        assert(fen_init_board(&board, fen, &err));

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        const U64 nodes = perft(&board, depth);
        clock_gettime(CLOCK_MONOTONIC, &end);

        ASSERT_UINT64_EQ(pert_result[depth], nodes, test_perft_pos_4);

        const long long diff_ns = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL +
              (end.tv_nsec - start.tv_nsec);

        printf("[PASS] test_perft_pos_4 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_4() != 0) return 1;
    printf("✅ All tests in perft_test_pos_4.c passed.\n");
    return 0;
}
