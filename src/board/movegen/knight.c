#include "private/board/movegen/knight.h"
#include "max/board/dir.h"

max_0x88_dir_t MAX_KNIGHT_MOVES[MAX_KNIGHT_MOVES_LEN] = {
    2 * MAX_0x88_DIR_UP + MAX_0x88_DIR_RIGHT,
    2 * MAX_0x88_DIR_UP + MAX_0x88_DIR_LEFT,

    2 * MAX_0x88_DIR_DOWN + MAX_0x88_DIR_RIGHT,
    2 * MAX_0x88_DIR_DOWN + MAX_0x88_DIR_LEFT,

    2 * MAX_0x88_DIR_RIGHT + MAX_0x88_DIR_UP,
    2 * MAX_0x88_DIR_RIGHT + MAX_0x88_DIR_DOWN,

    2 * MAX_0x88_DIR_LEFT * MAX_0x88_DIR_UP,
    2 * MAX_0x88_DIR_LEFT * MAX_0x88_DIR_DOWN,
};
