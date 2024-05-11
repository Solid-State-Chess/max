#include "max/board/board.h"

void max_board_check_unit_tests(void) {
    max_state_t buf[8];

    max_board_t board;
    max_board_new(&board, buf, MAX_ZOBRIST_DEFAULT_SEED);
}
