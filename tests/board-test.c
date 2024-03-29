#include "test.h"

#define MAX_CONSOLE
#include "max/board.h"

int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);

    max_board_debugprint(&board);

    return 1;
}
