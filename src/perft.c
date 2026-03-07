#include "move.h"
#include "movegen.h"
#include "perft.h"

U64 perft(Board *board, const U8 depth) {
    if (!is_king_alive(board, board->side_to_move)) {
        return 0ULL;
    }

    if (depth == 0) {
        return 1ULL;
    }

    MoveList legal_moves = {0};
    generate_all_legal_moves(board, &legal_moves);

    U64 nodes = 0ULL;

    for (size_t i = 0; i < legal_moves.count; i++) {
        const Move move = legal_moves.moves[i];
        StateInfo state = {0};
        make_move(board, move, &state);

        nodes += perft(board, depth - 1);

        unmake_move(board, &state);
    }

    return nodes;
}
