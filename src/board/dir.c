#include "max/board/dir.h"
#include "max/board/loc.h"

max_0x88_dir_t MAX_0x88_RAYS[MAX_0x88_RAYS_LEN] = {
    MAX_0x88_DIR_UP,
    MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_DOWN,
    MAX_0x88_DIR_LEFT,

    MAX_0x88_DIR_UR,
    MAX_0x88_DIR_UL,
    MAX_0x88_DIR_DR,
    MAX_0x88_DIR_DL,
};

max_0x88_dir_t *MAX_0x88_DIAGONALS = &MAX_0x88_RAYS[4];
max_0x88_dir_t *MAX_0x88_CARDINALS = &MAX_0x88_RAYS[0];

// 240 byte array giving the direction between two squares (if any) by the 0x88 difference index
// between them
static max_0x88_dir_t MAX_DIRECTION_BY_DIFF[MAX_0x88_DIFF_LEN] = {MAX_0x88_DIR_INVALID};

max_0x88_dir_t max_0x88_line(max_0x88_t from, max_0x88_t to) {
    max_0x88_diff_t diff = max_0x88_diff(from, to);
    return MAX_DIRECTION_BY_DIFF[diff.v];
}

static void max_0x88_init_line(uint8_t start, int8_t inc) {
    for(unsigned i = start; i != MAX_0x88_DIFF_CENTER; i += inc) {
        MAX_DIRECTION_BY_DIFF[i] = -inc;
    }
}

void max_0x88_init_static(void) {
    max_0x88_init_line(0xE7, MAX_0x88_DIR_DOWN);
    max_0x88_init_line(0x07, MAX_0x88_DIR_UP);
    max_0x88_init_line(0x70, MAX_0x88_DIR_RIGHT);
    max_0x88_init_line(0x7F, MAX_0x88_DIR_LEFT);


    max_0x88_init_line(0x00, MAX_0x88_DIR_UR);
    max_0x88_init_line(0x0E, MAX_0x88_DIR_UL);
    max_0x88_init_line(0xE0, MAX_0x88_DIR_DR);
    max_0x88_init_line(0xEE, MAX_0x88_DIR_DL);
}

#ifdef MAX_TESTS
#include "private/test.h"
#include "max/board/squares.h"

void max_0x88_dir_unit_tests(void) {
    max_0x88_dir_t dir;
    ASSERT(max_0x88_line(MAX_E2, MAX_E8) == MAX_0x88_DIR_UP, "Direction finding wrong, got %d", dir);
    ASSERT(max_0x88_line(MAX_D5, MAX_F7) == MAX_0x88_DIR_UR, "");
    ASSERT(max_0x88_line(MAX_A6, MAX_A4) == MAX_0x88_DIR_DOWN, "");
    ASSERT(max_0x88_line(MAX_A1, MAX_F6) == MAX_0x88_DIR_UR, "");
    ASSERT(max_0x88_line(MAX_F6, MAX_A1) == MAX_0x88_DIR_DL, "");
    ASSERT(max_0x88_line(MAX_E2, MAX_E2) == MAX_0x88_DIR_INVALID, "There is a line between the same square: %d", dir);
    ASSERT(max_0x88_line(MAX_F2, MAX_A5) == MAX_0x88_DIR_INVALID, "There is a line between two non-lined squares: %d", dir);
}
#endif
