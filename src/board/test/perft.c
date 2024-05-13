#include "max/board/perft.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "private/board/board.h"
#include "private/test.h"

#ifdef MAX_TESTS

uint64_t max_board_perft(max_board_t *board,  max_movelist_t moves, uint8_t depth) {

    if(depth == 0) {
        return 1;
    }
    
    max_board_movegen(board, &moves);
    
    uint64_t count = 0;
    unsigned legal_moves = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(max_board_legal(board, move)) {
            legal_moves += 1;

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
#define MAX_BOARD_PERFT_5 (4865609UL)
#define MAX_BOARD_PERFT_6 (119060324UL)

static const uint64_t EXPECTED_PERFT[] = {
    20,
    400,
    8902,
    197281,
    4865609,
    //119060324,
};

static const unsigned EXPECTED_PERFT_LEN = sizeof(EXPECTED_PERFT) / sizeof(EXPECTED_PERFT[0]);

void max_board_perft_unit_tests(void) {
    max_state_t state_buf[12];
    max_board_t board;
    max_board_new(&board, state_buf, MAX_ZOBRIST_DEFAULT_SEED);
    max_board_default_pos(&board);

    max_smove_t buf[MAX_BOARD_TEST_MOVELIST_LEN];
    max_movelist_t moves;
    max_movelist_new(&moves, buf, MAX_BOARD_TEST_MOVELIST_LEN);
    
    for(unsigned i = 0; i < EXPECTED_PERFT_LEN; ++i) {
        uint64_t perft = max_board_perft(&board, moves, i + 1);
        ASSERT(perft == EXPECTED_PERFT[i], "perft(%u) invalid - got %zu nodes, expecting %zu nodes", i + 1, perft,  EXPECTED_PERFT[i]);
    }
}


#endif
