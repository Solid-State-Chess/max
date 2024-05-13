#include "max/board/board.h"
#include "max/board/fen.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/squares.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/test.h"

#ifdef MAX_TESTS

void max_board_check_unit_tests(void) {
    max_state_t buf[8];

    max_board_t board;
    max_board_new(&board, buf, MAX_ZOBRIST_DEFAULT_SEED);
    
    max_fen_parse_err_t ec;
    ASSERT(
        (ec = max_board_parse_from_fen(&board, "rnbq1bnr/pp1pkppp/8/1Pp1p3/8/B7/P1PPPPPP/RN1QKBNR w KQ-- c6 0 1")) == MAX_FEN_SUCCESS,
        "Failed to parse FEN string: %s",
        max_fen_parse_err_str(ec)
    );

    max_board_make_move(&board, max_smove_new(MAX_B5, MAX_C6, MAX_MOVETAG_ENPASSANT));
    ASSERT(
        !max_check_is_empty(max_board_state(&board)->check[0]) &&
        max_check_is_sliding(max_board_state(&board)->check[0]),
        "Check is not detected when discovered via en passant"
    );
}

#endif
