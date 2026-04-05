#ifndef UCI_H
#define UCI_H

#include "board.h"

void uci_init(Board *board);
void uci_loop();
void parse_position(Board *board, const char *line);
void parse_go(const Board *board, const char *line);

#endif
