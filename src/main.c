#include "fen_parser.h"

int main(void) {
    printf("Bit-Chess\n");

    Board board;
    FenError error;
    if (fen_init_board(&board, FEN_START_POS, &error)) {
        printf("FEN initialized\n");
        print_board(&board);
    }

    return 0;
}
