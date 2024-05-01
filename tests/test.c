#include "max.h"
#include "max/board/board.h"


int main(int argc, char *argv[]) {
    static max_state_t buf[64] = {0};
    max_unit_tests();

    max_chessboard_t board;
    max_chessboard_new(&board, buf, 0xdeadbeef);
    max_chessboard_default_pos(&board);

    max_chessboard_print(&board);

    return 0;
}
