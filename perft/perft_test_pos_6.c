#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_6(void) {
    for (U8 depth = 0; depth <= 5; depth++) {
        printf("[RUN] test_perft_pos_6 for depth: %d...\n", depth);

        const char *fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
        const U64 pert_result[] = {1ULL, 46ULL, 2079ULL, 89890ULL, 3894594ULL, 164075551ULL,
            6923051137ULL, 287188994746ULL, 11923589843526ULL, 490154852788714ULL};

        Board board;
        FenError err;
        assert(fen_init_board(&board, fen, &err));

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        const U64 nodes = perft(&board, depth);
        clock_gettime(CLOCK_MONOTONIC, &end);

        ASSERT_UINT64_EQ(pert_result[depth], nodes, test_perft_pos_5);

        const long long diff_ns = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL +
              (end.tv_nsec - start.tv_nsec);

        printf("[PASS] test_perft_pos_6 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_6() != 0) return 1;
    printf("✅ All tests in perft_test_pos_6.c passed.\n");
    return 0;
}
