#include "max/board/board.h"
#include "private/board/zobrist.h"

void max_chesboard_new(max_chessboard_t *board, max_state_t *buffer, max_zobrist_t seed) {
    max_zobrist_elements_init(&board->zobrist_state, seed);
}
