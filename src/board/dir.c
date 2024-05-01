#include "max/board/dir.h"
#include "max/board/loc.h"

// 240 byte array giving the direction between two squares (if any) by the 0x88 difference index
// between them
static max_0x88_dir_t MAX_DIRECTION_BY_DIFF[MAX_0x88_DIFF_LEN];

bool max_0x88_line(max_0x88_t from, max_0x88_t to, max_0x88_dir_t *dir) {
    max_0x88_diff_t diff = max_0x88_diff(from, to);
    *dir = MAX_DIRECTION_BY_DIFF[diff.v];
    return dir->v != MAX_0x88_DIR_INVALID;
}
