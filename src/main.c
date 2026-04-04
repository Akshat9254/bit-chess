#include <stdlib.h>
#include <stdio.h>

#include "fen_parser.h"
#include "move.h"
#include "movegen.h"

int main(void) {
    printf("Bit-Chess\n");

    Board board;
    FenError error;
    StateInfo state;

    if (fen_init_board(&board, FEN_START_POS, &error)) {
        while (is_king_alive(&board, COLOR_WHITE) && is_king_alive(&board, COLOR_BLACK)) {
            print_board(&board);
            MoveList move_list = {0};
            generate_all_legal_moves(&board, &move_list);

            int choice = -1;
            char *line = nullptr;
            size_t len = 0;

            while (choice < 0 || choice >= (int) move_list.count) {
                for (size_t i = 0; i < move_list.count; i++) {
                    char str[6] = {0};
                    move_to_string(move_list.moves[i], board.side_to_move, str, 6);
                    printf("%zu: %s\n", (i + 1), str);
                }

                printf("Select move (1-%zu): ", move_list.count);

                if (getline(&line, &len, stdin) != -1) {
                    choice = atoi(line) - 1;
                    if (choice < 0 || choice >= (int) move_list.count) {
                        printf("Invalid option! Try again.\n");
                    }
                }
            }

            const Move move = move_list.moves[choice];
            make_move(&board, move, &state);

            free(line);
            print_board(&board);
        }
    }

    return 0;
}
