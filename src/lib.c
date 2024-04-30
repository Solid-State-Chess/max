#include "max.h"
#include "private/board/loc.h"
#include "private/test.h"
#include "private/board/move.h"

#ifdef MAX_TESTS
#include <stdio.h>

uint32_t _max_failed_tests = 0;
uint32_t _max_tests = 0;

void max_unit_tests(void) {
    CATEGORY(max_move_unit_tests, "move unit tests");
    CATEGORY(max_loc_unit_tests,  "board index unit tests");
}

#endif
