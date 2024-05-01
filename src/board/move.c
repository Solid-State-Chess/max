#include "max/board/move.h"
#include "max/board/loc.h"
#include "max/board/squares.h"
#include "private/board/move.h"
#include "private/test.h"


#ifdef MAX_TESTS

void max_move_unit_tests(void) {
    max_pmove_t packed = max_pmove_create_from(
        (max_smove_t) {
            .from = MAX_E2,
            .to = MAX_H8, 
            .tag = MAX_MOVETAG_CAPTURE | MAX_MOVETAG_PQUEEN
    });
    
    max_0x88_t target = max_pmove_from(packed);
    ASSERT(target.v == MAX_E2.v, "bad packed move destination square: %c%c", MAX_0x88_FORMAT(target));

    target = max_pmove_to(packed);
    ASSERT(target.v == MAX_H8.v, "bad packed move source square: %c%c", MAX_0x88_FORMAT(target));

    max_movetag_t tag = max_pmove_tag(packed);
    ASSERT(tag == (MAX_MOVETAG_CAPTURE | MAX_MOVETAG_PQUEEN), "bad packed move tag: %0X", tag);
}

#endif
