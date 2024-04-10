#include "max/engine.h"
#include "max/board.h"

void max_engine_new(max_engine_t *engine) {
    max_board_new(&engine->board, engine->board_stack);
}
