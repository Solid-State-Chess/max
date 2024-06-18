#include "max/board/loc.h"
#include "max/board/side.h"
#include "private/board/loc.h"
#include "private/test.h"


#ifdef MAX_TESTS

void max_loc_unit_tests(void) {
    max_0x88_t p1 = max_0x88_new(7, 1);
    max_6bit_t p2 =max_6bit_new(1, 7);

    ASSERT(max_0x88_rank(p1) == max_6bit_file(p2), "Invalid 0x88 or LSF unpacking");

    p1 = max_0x88_new(1, 7);
    max_6bit_t c1 = max_0x88_to_6bit(p1);
    ASSERT(c1.v == p2.v, "bad 0x88 to 6 bit LSF conversion");

    max_0x88_t c2 = max_6bit_to_0x88(p2);
    ASSERT(p1.v == c2.v, "bad 6 bit LSF to 0x88 conversion");

    
    p1 = max_0x88_new(2, 3);
    max_0x88_t mirror = max_0x88_new(7 - 2, 3);
    ASSERT(max_0x88_mirror_y(p1).v == mirror.v, "Rank mirroring invalid");
    ASSERT(max_0x88_mirror_side(p1, MAX_SIDE_BLACK).v == mirror.v, "0x88 rank mirroring by side incorrect for black square");
    ASSERT(max_0x88_mirror_side(p1, MAX_SIDE_WHITE).v == p1.v, "0x88 rank mirroring by side does not preserve white squares");
}

#endif
