#include "private/board/movegen/king.h"
#include "max/board/side.h"
#include "max/board/squares.h"


max_0x88_dir_t MAX_KING_MOVES[MAX_KING_MOVES_LEN] = {
    MAX_0x88_DIR_UP,
    MAX_0x88_DIR_UR,
    MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_DR,
    MAX_0x88_DIR_DOWN,
    MAX_0x88_DIR_DL,
    MAX_0x88_DIR_LEFT,
    MAX_0x88_DIR_UL,
};

max_0x88_t MAX_ACASTLE_KING_DEST[MAX_SIDES_LEN] = {
    MAX_C1,
    MAX_C8,
};

max_0x88_t MAX_HCASTLE_KING_DEST[MAX_SIDES_LEN] = {
    MAX_G1,
    MAX_G8,
};

max_0x88_t MAX_ACASTLE_ROOK_DEST[MAX_SIDES_LEN] = {
    MAX_D1,
    MAX_D8,
};

max_0x88_t MAX_HCASTLE_ROOK_DEST[MAX_SIDES_LEN] = {
    MAX_F1,
    MAX_F8,
};
