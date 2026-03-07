#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_3(void) {
    for (U8 depth = 1; depth <= 6; depth++) {
        printf("[RUN] test_perft_pos_3 for depth: %d...\n", depth);

        const char *fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
        const U64 pert_result[] = {0ULL, 14ULL, 191ULL, 2812ULL, 43238ULL, 674624ULL, 11030083ULL,
            178633661ULL, 3009794393ULL};

        Board board;
        FenError err;
        assert(fen_init_board(&board, fen, &err));

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        const U64 nodes = perft(&board, depth);
        clock_gettime(CLOCK_MONOTONIC, &end);

        ASSERT_UINT64_EQ(pert_result[depth], nodes, test_perft_pos_3);

        const long long diff_ns = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL +
              (end.tv_nsec - start.tv_nsec);

        printf("[PASS] test_perft_pos_3 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_3() != 0) return 1;
    printf("✅ All tests in perft_test_pos_3.c passed.\n");
    return 0;
}
