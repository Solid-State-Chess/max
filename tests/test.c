#include "max.h"
#include "max/board/board.h"


int main(int argc, char *argv[]) {
    static max_state_t buf[64] = {0};
    max_init();

    max_unit_tests();

    max_board_t board;
    max_board_new(&board, buf, 0xfa3198db566d5520);
    max_board_default_pos(&board);

    max_board_print(&board);

    return 0;
}
