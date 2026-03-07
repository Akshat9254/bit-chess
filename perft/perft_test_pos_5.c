#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_5(void) {
    for (U8 depth = 1; depth <= 5; depth++) {
        printf("[RUN] test_perft_pos_5 for depth: %d...\n", depth);

        const char *fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
        const U64 pert_result[] = {0ULL, 44ULL, 1486ULL, 62379ULL, 2103487ULL, 89941194ULL};

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

        printf("[PASS] test_perft_pos_5 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_5() != 0) return 1;
    printf("✅ All tests in perft_test_pos_5.c passed.\n");
    return 0;
}
