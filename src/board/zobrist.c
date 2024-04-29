#include "max/board/zobrist.h"
#include "max/board/board.h"
#include <stdlib.h>


max_zobrist_t MAX_ZOBRIST_PIECEPOS[12][64] = {0};

max_zobrist_t MAX_ZOBRIST_CASTLERIGHTS[16] = {0};

max_zobrist_t MAX_ZOBRIST_EPFILE[8] = {0};

/// Generate a random number to be used in zobrist key creation
max_zobrist_t max_zobrist_rand(max_board_t *const board) {
    return rand();
}
