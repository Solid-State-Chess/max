#define MAX_CONSOLE
#include "max.h"
#include "max/move.h"
#include "test.h"
#include "max/board.h"

int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);

    max_board_debugprint(&board);

    max_movelist_t moves;
    max_movelist_new(&moves);

    max_movegen(&moves, &board);
    for(unsigned i = 0; i < moves.len; ++i) {
        max_square_idx_t from = moves.moves[i].from;
        max_square_idx_t to   = moves.moves[i].to;
        printf("MOVE (%d, %d) -> (%d, %d)\n", from % 10, from / 10 - 1, to % 10, to / 10 - 1);
    }

    return 1;
}
