#include "max/board/piece.h"
#include "max/engine/tt.h"
#include "private.h"

/// Get a Most-Valuable Victim / Least Valuable Aggressor score to sort capture moves by
static max_score_t max_engine_mvvlva(max_engine_t *engine, max_move_t move) {
    return max_piecevalue(engine, engine->board.pieces[move.to]) - max_piecevalue(engine, engine->board.pieces[move.from]);
}

void max_engine_sortmoves(max_engine_t *engine, max_movelist_t *moves) {
    uint8_t head = 0;

    max_ttentry_t* entry = max_ttbl_probe(&engine->tt, engine->board.zobrist.hash); 
    if(entry) {
        for(unsigned i = 0; i < moves->len; ++i) {
            max_move_t move = moves->moves[i];
            if(max_move_equal(entry->move, move)) {
                moves->moves[i] = moves->moves[head];
                moves->moves[head] = move;
                engine->diagnostic.tt_hits += 1;
                head += 1;
                break;
            }
        }
    }
    
    uint8_t captures = head;
    for(unsigned i = head; i < moves->len; ++i) {
        max_move_t move = moves->moves[i];
        if(
            engine->board.pieces[move.to] != MAX_PIECECODE_EMPTY
        ) {
            moves->moves[i] = moves->moves[head];
            moves->moves[head] = move;
            head += 1;
        }
    }
    
    if(head > 1 && head - captures > 2) {
        for(;;) {
            bool sorted = true;
            for(unsigned i = captures; i < head - 1; ++i) {
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
}
