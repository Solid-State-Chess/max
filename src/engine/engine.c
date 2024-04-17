#include "max/engine/engine.h"
#include "max/board/board.h"

void max_engine_new(max_engine_t *engine) {
    max_board_new(&engine->board, engine->board_stack);
    max_ttbl_new(&engine->tt);
}
