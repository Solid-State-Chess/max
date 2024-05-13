#include "max.h"
#include "max/board/board.h"
#include "max/board/move.h"
#include "max/board/movegen.h"


int main(int argc, char *argv[]) {
    max_smove_t movebuf[1024] = {0};
    static max_state_t buf[64] = {0};
    max_init();

    max_unit_tests();

    max_board_t board;
    max_board_new(&board, buf, 0xfa3198db566d5520);
    max_board_default_pos(&board);

    max_board_print(&board);

    max_movelist_t moves;
    max_movelist_new(&moves, movebuf, 1024);
    max_board_movegen(&board, &moves);


    return 0;
}