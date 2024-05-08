#include "max/board/move.h"
#include "max/board/movegen.h"
#include "private/board/board.h"
#include "private/test.h"
#include <string.h>

#ifdef MAX_TESTS

void max_board_perft(max_board_t *board, max_perft_count_t *count, max_movelist_t moves, uint8_t depth) {
    if(depth == 0) {
        count->nodes += 1;
        return;
    }
    
    max_board_t old;
    memcpy(&old, board, sizeof(*board));

    max_board_movegen(board, &moves);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(max_board_legal(board, move)) {
            if(move.tag == MAX_MOVETAG_ENPASSANT) {
                count->en_passants += 1;
            } else if(move.tag & MAX_MOVETAG_CAPTURE) {
                count->captures += 1;
            } else if(move.tag == MAX_MOVETAG_ACASTLE || move.tag == MAX_MOVETAG_HCASTLE) {
                count->castles += 1;
            }

            max_board_make_move(board, move);
            max_board_perft(board, count, max_movelist_slice(&moves), depth - 1);
            max_board_unmake_move(board, move);
        }
    }
}

void max_perft_count_print(max_perft_count_t *count) {
    printf(
        "    Nodes: %zu     \n"
        "    Captures: %zu  \n"
        "    En Passant: %zu\n"
        "    Castles: %zu   \n"
        "    Checks: %zu    \n",
        count->nodes,
        count->captures,
        count->en_passants,
        count->castles,
        count->checks
    );
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
    
    max_perft_count_t count = max_perft_count_default();
    max_board_perft(&board, &count, moves, 2);
    ASSERT(count.nodes == MAX_BOARD_PERFT_2, "perft(2) invalid - expecting %zu nodes\n", MAX_BOARD_PERFT_2);
    
    count = max_perft_count_default();
    max_board_perft(&board, &count, moves, 3);
    ASSERT(count.nodes == MAX_BOARD_PERFT_3, "perft(3) invalid - expecting %zu nodes\n", MAX_BOARD_PERFT_3);
}


#endif
