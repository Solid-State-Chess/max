#include "max/board/board.h"
#include "max/board/fen.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/squares.h"
#include "max/board/zobrist.h"
#include "private/test.h"

#ifdef MAX_TESTS

void max_board_legality_unit_tests(void) {
    max_state_t buf[10];
    max_board_t board;
    max_board_new(&board, buf, MAX_ZOBRIST_DEFAULT_SEED);

    ASSERT(
        max_board_parse_from_fen(&board, "k7/8/8/r2pPK2/8/8/8/8 w - d6 0 1") == MAX_FEN_SUCCESS,
        "FEN parse when setting up legality unit test fails"
    );

    max_smove_t testmove = max_smove_new(MAX_E5, MAX_D6, MAX_MOVETAG_ENPASSANT);
    ASSERT(
        !max_board_legal(&board, testmove),
        "En passant with a horizontally pinned pawn is allowed"
    );

    ASSERT(
        max_board_parse_from_fen(&board, "rnbqkbnr/pp1ppppp/8/2p5/5P2/3P4/PPP1P1PP/RNBQKBNR b KQkq - 0 1") == MAX_FEN_SUCCESS,
        "FEN parse when setting up legality unit test fails"
    );

    max_board_make_move(&board, max_smove_normal(MAX_D8, MAX_A5));

    ASSERT(
        max_board_legal(&board, max_smove_normal(MAX_E1, MAX_F2)),
        "King move escaping sliding check is not marked legal"
    );
}

#endif
