#include <pthread.h>
#include <stdio.h>

#include "uci.h"
#include "bit_search.h"
#include "fen_parser.h"
#include "move.h"
#include "utils.h"

StateInfo state_info;

typedef struct {
    pthread_t thread_id;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    bool running;
    bool stopped;
    Board *board;
} SearchWorker;

SearchWorker search_worker;

static void set_time_limits(const Board *board, int wtime, int winc, int btime, int binc, int move_time, U8 moves_to_go);
static void* search_worker_loop(void *arg);
static void stop_search(void);

void uci_init(Board *board) {
    search_worker.running = false;
    search_worker.stopped = false;
    search_worker.board = board;
    pthread_mutex_init(&search_worker.lock, nullptr);
    pthread_cond_init(&search_worker.cond, nullptr);
    pthread_create(&search_worker.thread_id, nullptr, search_worker_loop, nullptr);
}

void uci_loop() {
    const Board *board = search_worker.board;
    char line[2048];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (strncmp(line, "uci", 3) == 0) {
            printf("id name BitChess v1.0\n");
            printf("id author Akshat Pandey\n");
            printf("uciok\n");
        } else if (strncmp(line, "isready", 5) == 0) {
            printf("readyok\n");
        } else if (strncmp(line, "position", 8) == 0) {
            stop_search();
            parse_position(search_worker.board, line);
        } else if (strncmp(line, "go", 2) == 0) {
            stop_search();
            parse_go(board, line);
        } else if (strncmp(line, "d", 1) == 0) {
            print_board(board);
        } else if (strncmp(line, "stop", 4) == 0) {
            search_info.aborted = true;
        } else if (strncmp(line, "quit", 4) == 0) {
            stop_search();

            pthread_mutex_lock(&search_worker.lock);
            search_worker.stopped = true;
            pthread_cond_signal(&search_worker.cond);
            pthread_mutex_unlock(&search_worker.lock);

            pthread_join(search_worker.thread_id, nullptr);
            pthread_mutex_destroy(&search_worker.lock);
            pthread_cond_destroy(&search_worker.cond);
            break;
        }

        fflush(stdout);
    }
}

void parse_position(Board *board, const char *line) {
    // move line ptr to "position " -> 9 position
    line += 9;
    const char *ptr;
    FenError err;
    bool fen_result = false;
    if (strncmp(line, "startpos", 8) == 0) {
        fen_result = fen_init_board(board, FEN_START_POS, &err);
    } else if ((ptr = strstr(line, "fen"))) {
        ptr += 4;
        fen_result = fen_init_board(board, ptr, &err);
    }

    if (!fen_result) {
        perror(err.message);
    }

    ptr = strstr(line, "moves");

    if (ptr != NULL) {
        // moves ptr to "moves " -> 6 position
        ptr += 6;

        while (ptr != NULL && *ptr != '\0') {
            const Move move = move_from_string(ptr, board);
            make_move(board, move, &state_info);

            while (*ptr != '\0' && *ptr != ' ') {
                ptr++;
            }

            if (*ptr == ' ') {
                ptr++;
            }
        }
    }
}

void parse_go(const Board *board, const char *line) {
    int wtime = -1, winc = 0;
    int btime = -1, binc = 0;
    U32 depth = MAX_DEPTH;
    U8 moves_to_go = 30;
    int move_time = -1;
    char *ptr;

    if (strstr(line, "infinite")) {
        search_info.soft_limit = 1 << 30;
        search_info.hard_limit = 1 << 30;
    }

    if ((ptr = strstr(line, "depth"))) {
        depth = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "movetime"))) {
        move_time = atoi(ptr + 9);
    }

    if ((ptr = strstr(line, "wtime"))) {
        wtime = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "winc"))) {
        winc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "btime"))) {
        btime = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "binc"))) {
        binc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "movestogo"))) {
        moves_to_go = atoi(ptr + 10);
    }

    set_time_limits(board, wtime, winc, btime, binc, move_time, moves_to_go);

    pthread_mutex_lock(&search_worker.lock);
    search_info.depth = depth;
    search_info.start_time = get_time_ms();
    search_info.nodes = 0;
    search_info.aborted = false;
    search_worker.running = true;

    pthread_cond_signal(&search_worker.cond);
    pthread_mutex_unlock(&search_worker.lock);
}

static void set_time_limits(const Board *board, const int wtime, const int winc,
    const int btime, const int binc, const int move_time, const U8 moves_to_go) {
    if (move_time != -1) {
        search_info.soft_limit = (U32) (move_time * 0.8);
        search_info.hard_limit = (U32) (move_time * 0.95);
        return;
    }

    const int my_time = board->side_to_move == COLOR_WHITE ? wtime : btime;
    const int my_inc = board->side_to_move == COLOR_WHITE ? winc : binc;

    if (my_time != -1) {
        const U32 target_time = (my_time / (moves_to_go > 0 ? moves_to_go : 30)) + (U32)(my_inc * 0.8);
        search_info.soft_limit = (U32) (target_time * 0.4);
        search_info.hard_limit = target_time;

        const U32 max_safety = my_time / 5;
        if (search_info.hard_limit > max_safety) {
            search_info.hard_limit = max_safety;
        }

        return;
    }

    search_info.soft_limit = 1 << 30;
    search_info.hard_limit = 1 << 30;
}

static void* search_worker_loop(__attribute__((unused)) void *arg) {
    while (true) {
        pthread_mutex_lock(&search_worker.lock);

        while (!search_worker.running && !search_worker.stopped) {
            pthread_cond_wait(&search_worker.cond, &search_worker.lock);
        }

        if (search_worker.stopped) {
            pthread_mutex_unlock(&search_worker.lock);
            break;
        }

        pthread_mutex_unlock(&search_worker.lock);

        const Move best_move = search_for_best_move(search_worker.board);
        char move_str[6] = {0};
        move_to_string(best_move, search_worker.board->side_to_move, move_str, 6);
        printf("bestmove %s\n", move_str);
        fflush(stdout);

        pthread_mutex_lock(&search_worker.lock);
        search_worker.running = false;
        pthread_cond_signal(&search_worker.cond);
        pthread_mutex_unlock(&search_worker.lock);
    }

    return nullptr;
}

static void stop_search(void) {
    search_info.aborted = true;

    pthread_mutex_lock(&search_worker.lock);
    while (search_worker.running) {
        pthread_cond_wait(&search_worker.cond, &search_worker.lock);
    }

    pthread_mutex_unlock(&search_worker.lock);
}
