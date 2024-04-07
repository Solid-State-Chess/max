#include "max/piece.h"
#include "private.h"

/// Get a Most-Valuable Victim / Least Valuable Aggressor score to sort capture moves by
max_score_t max_engine_mvvlva(max_engine_t *engine, max_move_t move) {
    return max_piecevalue(engine, engine->board.pieces[move.to]) - max_piecevalue(engine, engine->board.pieces[move.from]);
}

void max_engine_sortmoves(max_engine_t *engine, max_movelist_t *moves) {
    uint8_t capture_len = 0;
    for(unsigned i = 0; i < moves->len; ++i) {
        max_move_t move = moves->moves[i];
        if(
            engine->board.pieces[move.to] != MAX_PIECECODE_EMPTY
        ) {
            moves->moves[i] = moves->moves[capture_len];
            moves->moves[capture_len] = move;
            capture_len += 1;
        }
    }
    
    if(capture_len <= 1 || moves->len <= 2) {
        return;
    }

    for(;;) {
        bool sorted = true;
        for(unsigned i = 0; i < capture_len - 1; ++i) {
            max_move_t move = moves->moves[i];
            max_move_t next = moves->moves[i + 1];
            if(max_engine_mvvlva(engine, move) < max_engine_mvvlva(engine, next)) {
                moves->moves[i] = next;
                moves->moves[i + 1] = move;
                i += 1;
                sorted = false;
                break;
            }
        }

        if(sorted) {
            break;
        }
    }
}
