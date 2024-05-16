#include "max.h"
#include "max/assert.h"
#include "max/engine/engine.h"
#include "max/engine/score.h"
#include "private/board/board.h"
#include "private/board/loc.h"
#include "private/board/piececode.h"
#include "private/board/piecelist.h"
#include "private/test.h"
#include "private/board/move.h"
#include "private/board/dir.h"

#include <stdlib.h>
#include <valgrind/callgrind.h>

#ifdef MAX_ASSERTS
bool MAX_INITIALIZED = false;
#endif

void max_init(void) {
    MAX_ASSERT(!MAX_INITIALIZED && "max_init() called twice");
#ifdef MAX_ASSERTS
    MAX_INITIALIZED = true;
#endif
    max_0x88_init_static();
}

#ifdef MAX_TESTS

uint32_t _max_failed_tests = 0;
uint32_t _max_tests = 0;

void max_unit_tests(void) {
    /*CATEGORY(max_move_unit_tests, "move unit tests");
    CATEGORY(max_loc_unit_tests,  "board index unit tests");
    CATEGORY(max_0x88_dir_unit_tests, "0x88 direction unit tests");
    CATEGORY(max_board_tests, "board unit tests");
    CATEGORY(max_pieces_unit_tests, "piece list unit tests");
    CATEGORY(max_piececode_unit_tests, "piece code unit tests");*/

    static const unsigned BOARD_STACK_CAP  = 24;
    static const unsigned MOVELIST_CAP     = 26 * MAX_ENGINE_MAX_MOVES_PER_PLY;
    static const unsigned TTBL_BUF_CAP_BIT = 20;
    static const unsigned TTBL_BUF_CAP     = (1 << TTBL_BUF_CAP_BIT);
    
    max_engine_init_params_t init = (max_engine_init_params_t){
        .board = {
            .stack = malloc(sizeof(max_state_t) * BOARD_STACK_CAP),
            .capacity = BOARD_STACK_CAP
        },
        .ttbl = {
            .buf = malloc(sizeof(max_ttentry_t) * TTBL_BUF_CAP),
            .nbit = TTBL_BUF_CAP_BIT
        },
        .moves = {
            .buf = malloc(sizeof(max_smove_t) * MOVELIST_CAP),
            .capacity = MOVELIST_CAP
        }
    };
    
    max_engine_t engine;
    max_engine_new(&engine, &init, max_eval_params_default());
    max_board_default_pos(&engine.board);
    
    max_search_result_t search;

    CALLGRIND_START_INSTRUMENTATION;
    max_engine_search(&engine, &search);
    CALLGRIND_STOP_INSTRUMENTATION;
}

#endif
