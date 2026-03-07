#include <time.h>

#include "perft.h"
#include "fen_parser.h"
#include "test_util.h"

static int test_perft_pos_2(void) {
    for (U8 depth = 1; depth <= 5; depth++) {
        printf("[RUN] test_perft_pos_2 for depth: %d...\n", depth);

        const char *fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
        const U64 pert_result[] = {0ULL, 48ULL, 2039ULL, 97862ULL, 4085603ULL, 193690690ULL, 8031647685ULL};
        Board board;
        FenError err;
        assert(fen_init_board(&board, fen, &err));

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        const U64 nodes = perft(&board, depth);
        clock_gettime(CLOCK_MONOTONIC, &end);

        ASSERT_UINT64_EQ(pert_result[depth], nodes, test_perft_pos_2);

        const long long diff_ns = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL +
              (end.tv_nsec - start.tv_nsec);

        printf("[PASS] test_perft_pos_2 for depth: %d passed with execution time: %lldns\n", depth, diff_ns);
    }

    return 0;
}

int main(void) {
    if (test_perft_pos_2() != 0) return 1;
    printf("✅ All tests in perft_test_pos_2.c passed.\n");
    return 0;
}
