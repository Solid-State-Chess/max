#include "max/piece.h"
#include "private.h"

void max_engine_sortmoves(max_engine_t *engine, max_movelist_t *moves) {
    uint8_t capture_len = 0;
    for(unsigned i = 0; i < moves->len; ++i) {
        max_move_t move = moves->moves[i];
        if(engine->board.pieces[move.to] != MAX_PIECECODE_EMPTY) {
            moves->moves[i] = moves->moves[capture_len];
            moves->moves[capture_len] = move;
            capture_len += 1;
        }
    }
}
