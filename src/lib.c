#include "max.h"
#include "max/assert.h"
#include "private/board/loc.h"
#include "private/max.h"
#include "private/test.h"
#include "private/board/move.h"
#include "private/board/dir.h"

#ifdef MAX_ASSERTS
bool MAX_INITIALIZED = false;
#endif

void max_init(void) {
    MAX_ASSERT(!MAX_INITIALIZED && "max_init() called twice");
#ifdef MAX_ASSERTS
    MAX_INITIALIZED = true;
#endif
    max_0x88_init_static();
}

#ifdef MAX_TESTS

uint32_t _max_failed_tests = 0;
uint32_t _max_tests = 0;

void max_unit_tests(void) {
    CATEGORY(max_move_unit_tests, "move unit tests");
    CATEGORY(max_loc_unit_tests,  "board index unit tests");
    CATEGORY(max_0x88_dir_unit_tests, "0x88 difference unit tests");
}

#endif
