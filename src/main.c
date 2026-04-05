#include "uci.h"

int main(void) {
    Board board;
    uci_init(&board);
    uci_loop();

    return 0;
}
