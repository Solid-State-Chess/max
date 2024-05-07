#include "max/board/movegen.h"
#include "private/board/board.h"
#include "private/test.h"

#ifdef MAX_TESTS

uint64_t max_board_perft(max_board_t *board, max_movelist_t moves, uint8_t depth) {
    if(depth == 0) {
        return 1;
    }

    uint64_t count = 0;
    max_board_movegen(board, &moves);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(max_board_legal(board, move)) {
            max_board_make_move(board, move);
            count += max_board_perft(board, max_movelist_slice(&moves), depth - 1);
            max_board_unmake_move(board, move);
        }
    }

    return count;
}

#define MAX_BOARD_TEST_MOVELIST_LEN (1024)

#define MAX_BOARD_PERFT_2 (400UL)
#define MAX_BOARD_PERFT_3 (8902UL)

void max_board_tests(void) {
    max_state_t state_buf[12];
    max_board_t board;
    max_board_new(&board, state_buf, 0xfa3198db566d5520);
    max_board_default_pos(&board);

    max_smove_t buf[MAX_BOARD_TEST_MOVELIST_LEN];
    max_movelist_t moves;
    max_movelist_new(&moves, buf, MAX_BOARD_TEST_MOVELIST_LEN);

    uint64_t nodes = max_board_perft(&board, moves, 2);
    ASSERT(nodes == MAX_BOARD_PERFT_2, "perft(2) invalid - got %zu nodes, expecting %zu\n", nodes, MAX_BOARD_PERFT_2);

    nodes = max_board_perft(&board, moves, 3);
    ASSERT(nodes == MAX_BOARD_PERFT_3, "perft(3) invalid - got %zu nodes, expecting %zu\n", nodes, MAX_BOARD_PERFT_3);
}


#endif
