#define MAX_CONSOLE
#include "max.h"
#include "max/move.h"
#include "test.h"
#include "max/board.h"


size_t board_recur(max_board_t *board, unsigned n) {
    size_t count = 0;
    if(n == 0) { return 1; }
    max_movelist_t moves;
    max_movelist_new(&moves);
    
    max_movegen(&moves, board);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_board_make_move(board, moves.moves[i]);
        count += board_recur(board, n - 1);
        max_board_unmake_move(board, moves.moves[i]);
    }

    return count;
}

int board_tests(void) {
    max_board_t board;
    max_board_reset(&board);
    
    board.ply += 1;
    max_movelist_t moves;
    max_movelist_new(&moves);
    size_t count = board_recur(&board, 3);

    max_board_debugprint(&board);

    printf("%zu POSITIONS\n", count);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_square_idx_t from = moves.moves[i].from;
        max_square_idx_t to   = moves.moves[i].to;
        printf("MOVE (%d, %d) -> (%d, %d)\n", from % 10, from / 10 - 1, to % 10, to / 10 - 1);
    }

    return 1;
}
