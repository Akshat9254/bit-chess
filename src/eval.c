#include "eval.h"

static const int PIECE_VALUES[] = {
    [WHITE_PAWN] = 100,     [BLACK_PAWN] = -100,
    [WHITE_KNIGHT] = 320,   [BLACK_KNIGHT] = -320,
    [WHITE_BISHOP] = 330,   [BLACK_BISHOP] = -330,
    [WHITE_ROOK] = 500,     [BLACK_ROOK] = -500,
    [WHITE_QUEEN] = 900,    [BLACK_QUEEN] = -900,
    [WHITE_KING] = 10000,   [BLACK_KING] = -10000,
    [PIECE_NONE] = 0
};

int eval(const Board *board) {
    int score = 0;
    for (Piece piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        score += PIECE_VALUES[piece] * bb_popcount(board->pieces[piece]);
    }

    return score;
}
