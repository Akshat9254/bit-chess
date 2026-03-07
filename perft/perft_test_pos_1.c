#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_1(void) {
    for (U8 depth = 0; depth <= 6; depth++) {
        printf("[RUN] test_perft_pos_1 for depth: %d...\n", depth);

        const U64 perft_result[] = {1ULL, 20ULL, 400ULL, 8902ULL, 197281ULL, 4865609ULL, 119060324ULL,
            3195901860ULL, 84998978956ULL, 2439530234167ULL, 69352859712417ULL};
        Board board;
        FenError err;
        assert(fen_init_board(&board, FEN_START_POS, &err));

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        const U64 nodes = perft(&board, depth);
        clock_gettime(CLOCK_MONOTONIC, &end);

        ASSERT_UINT64_EQ(perft_result[depth], nodes, test_perft_start_pos);

        const long long diff_ns = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL +
              (end.tv_nsec - start.tv_nsec);

        printf("[PASS] test_perft_pos_1 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_1() != 0) return 1;
    printf("✅ All tests in perft_test_pos_1.c passed.\n");
    return 0;
}
