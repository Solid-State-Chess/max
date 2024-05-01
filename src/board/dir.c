#include "max/board/dir.h"
#include "max/board/loc.h"

// 240 byte array giving the direction between two squares (if any) by the 0x88 difference index
// between them
static max_0x88_dir_t MAX_DIRECTION_BY_DIFF[MAX_0x88_DIFF_LEN] = {MAX_0x88_DIR_INVALID};

bool max_0x88_line(max_0x88_t from, max_0x88_t to, max_0x88_dir_t *dir) {
    max_0x88_diff_t diff = max_0x88_diff(from, to);
    *dir = MAX_DIRECTION_BY_DIFF[diff.v];
    return dir->v != MAX_0x88_DIR_INVALID;
}

static void max_0x88_init_line(uint8_t start, int8_t inc) {
    for(unsigned i = start; i != MAX_0x88_DIFF_CENTER; i += inc) {
        MAX_DIRECTION_BY_DIFF[i].v = -inc;
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
    ASSERT(max_0x88_line(MAX_E2, MAX_E8, &dir) && dir.v == MAX_0x88_DIR_UP, "Direction finding wrong, got %d", dir.v);
    ASSERT(max_0x88_line(MAX_D5, MAX_F7, &dir) && dir.v == MAX_0x88_DIR_UR, "");
    ASSERT(max_0x88_line(MAX_A6, MAX_A4, &dir) && dir.v == MAX_0x88_DIR_DOWN, "");
    ASSERT(max_0x88_line(MAX_A1, MAX_F6, &dir) && dir.v == MAX_0x88_DIR_UR, "");
    ASSERT(max_0x88_line(MAX_F6, MAX_A1, &dir) && dir.v == MAX_0x88_DIR_DL, "");
    ASSERT(!max_0x88_line(MAX_E2, MAX_E2, &dir), "");
}
#endif
