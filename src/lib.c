#include "max.h"
#include "private/test.h"
#include "private/board/move.h"
#include <stdio.h>

#ifdef MAX_TESTS

uint32_t _max_failed_tests = 0;
uint32_t _max_tests = 0;

void max_unit_tests(void) {
    CATEGORY(max_move_unit_tests, "Move Unit Test");
}

#endif
