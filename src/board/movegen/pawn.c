#include "private/board/movegen/pawn.h"
#include "max/board/dir.h"


max_0x88_dir_t MAX_PAWN_ATTACK_SIDES[MAX_PAWN_ATTACK_SIDE_LEN] = {
    MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_LEFT,
};

max_0x88_dir_t MAX_PAWN_ADVANCE_DIR[MAX_SIDES_LEN] = {
    MAX_0x88_DIR_UP,
    MAX_0x88_DIR_DOWN,
};
