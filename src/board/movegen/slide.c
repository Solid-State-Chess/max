#include "private/board/movegen/slide.h"
#include "max/board/dir.h"
#include "private/board/movegen.h"

void max_board_movegen_slide(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t source, max_0x88_dir_t ray) {
    max_0x88_t dest = source;
    for(;;) {
        dest = max_0x88_move(dest, ray);
        if(!max_board_movegen_attack(board, list, enemy, source, dest)) {
            return;
        }
    }
}
