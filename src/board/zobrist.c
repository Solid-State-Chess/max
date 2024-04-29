#include "max/board/zobrist.h"
#include "max/board/board.h"


max_zobrist_t MAX_ZOBRIST_PIECEPOS[12][64] = {0};

max_zobrist_t MAX_ZOBRIST_CASTLERIGHTS[16] = {0};

max_zobrist_t MAX_ZOBRIST_EPFILE[8] = {0};


#define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))

uint64_t ranval(max_zobrist_state_t *x) {
    uint8_t e = x->a - rot(x->b, 7);
    x->a = x->b ^ rot(x->c, 13);
    x->b = x->c + rot(x->d, 37);
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

/// Generate a random number to be used in zobrist key creation
max_zobrist_t max_zobrist_rand(max_board_t *const board) {
    return (max_zobrist_t)(ranval(&board->zobrist) >> 32);
}
