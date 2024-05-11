#include "max/board/move.h"
#include "max/board/movegen.h"
#include "private/board/board.h"
#include "private/test.h"
#include <string.h>

#ifdef MAX_TESTS

void max_board_perft(max_board_t *board, max_perft_count_t *count, max_movelist_t moves, uint8_t depth) {
    if(!max_check_is_empty(max_board_state(board)->check[0])) {
        count->checks += 1;
    }
    
    if(depth == 0) {
        count->nodes += 1;
        
        if(!max_check_is_empty(max_board_state(board)->check[0])) {
            max_board_movegen(board, &moves);
            unsigned checkmate = 1;
            for(unsigned i = 0; i < moves.len; ++i) {
                if(max_board_legal(board, moves.buf[i])) {
                    checkmate = 0;
                }
            }

            count->checkmates += checkmate;
        }
        return;
    }
    
    max_board_t old;
    memcpy(&old, board, sizeof(*board));

    max_board_movegen(board, &moves);
    
    unsigned legal_moves = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(max_board_legal(board, move)) {
            legal_moves += 1;
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

    if(legal_moves == 0 && !max_check_is_empty(max_board_state(board)->check[0])) {
        count->checkmates += 1;
        count->nodes += 1;
    }
}

void max_perft_count_print(max_perft_count_t const *count) {
    printf(
        "    Nodes: %zu     \n"
        "    Captures: %zu  \n"
        "    En Passant: %zu\n"
        "    Castles: %zu   \n"
        "    Checks: %zu    \n"
        "    Checkmates: %zu\n",
        count->nodes,
        count->captures,
        count->en_passants,
        count->castles,
        count->checks,
        count->checkmates
    );
}

static bool max_perft_count_eq(max_perft_count_t const *c1, max_perft_count_t const *c2) {
    return memcmp(c1, c2, sizeof(*c1)) == 0;
}

#define MAX_BOARD_TEST_MOVELIST_LEN (1024)

#define MAX_BOARD_PERFT_2 (400UL)
#define MAX_BOARD_PERFT_3 (8902UL)
#define MAX_BOARD_PERFT_5 (4865609UL)
#define MAX_BOARD_PERFT_6 (119060324UL)

static const max_perft_count_t EXPECTED_PERFT[] = {
    (max_perft_count_t){
        .nodes = 20,
        .captures = 0,
        .en_passants = 0,
        .castles = 0,
        .checks = 0,
        .checkmates = 0,
    },
    (max_perft_count_t){
        .nodes = 400,
        .captures = 0,
        .en_passants = 0,
        .castles = 0,
        .checks = 0,
        .checkmates = 0,
    },
    (max_perft_count_t){
        .nodes = 8902,
        .captures = 34,
        .en_passants = 0,
        .castles = 0,
        .checks = 12,
        .checkmates = 0,
    },
    (max_perft_count_t){
        .nodes = 197281,
        .captures = 1576,
        .en_passants = 0,
        .castles = 0,
        .checks = 469,
        .checkmates = 8,
    },
    (max_perft_count_t){
        .nodes =  4865609,
        .captures = 82719,
        .en_passants = 258,
        .castles = 0,
        .checks = 27351,
        .checkmates = 347,
    },
    (max_perft_count_t){
        .nodes = 119060324,
        .captures = 2812008,
        .en_passants = 5248,
        .castles = 0,
        .checks = 809099,
        .checkmates = 347,
    }
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
        max_perft_count_t count = max_perft_count_default();
        max_board_perft(&board, &count, moves, i + 1);
        bool pass = max_perft_count_eq(&count, &EXPECTED_PERFT[i]);
        ASSERT(pass, "perft(%u) invalid", i + 1);
        if(!pass) {
            printf("perft(%u) EXPECTED\n", i + 1);
            max_perft_count_print(&EXPECTED_PERFT[i]);
            printf("perft(%u) ACTUAL\n", i + 1);
            max_perft_count_print(&count);
            puts("\n\n");
        }
    }
}


#endif
