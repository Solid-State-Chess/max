#include "max/board/board.h"
#include "max/board/loc.h"
#include "private/board/capturelist.h"
#include "private/board/piecelist.h"
#include "private/board/state.h"
#include "private/board/zobrist.h"

static void max_chessboard_init_pieces(max_chessboard_t *board) {
    for(unsigned i = 0; i < MAX_0x88_LEN; ++i) {
        board->pieces[i].v = MAX_PIECECODE_INVALID;
    }
    
    for(unsigned i = 0; i < MAX_6BIT_LEN; ++i) {
        board->pieces[max_6bit_to_0x88(max_6bit_raw(i)).v].v = MAX_PIECECODE_EMPTY;
    }
}


void max_chesboard_new(max_chessboard_t *board, max_state_t *buffer, uint64_t seed) {
    max_zobrist_elements_init(&board->zobrist_state, seed);
    max_chessboard_init_pieces(board);

    max_plist_new(&board->side.white);
    max_plist_new(&board->side.black);

    max_captures_new(&board->captures);
    max_state_stack_new(&board->stack, buffer, max_state_default());

    board->ply = 0;
}
