#include "max/board/piecelist.h"
#include "private/board/piecelist.h"

#ifdef MAX_CONSOLE
#include <stdio.h>

static void max_loclist_print(char const *lbl, max_loclist_t *list) {
    fputs(lbl, stdout);
    fputs(": ", stdout);
    for(unsigned i = 0; i < list->len; ++i) {
        printf("%c%c ", MAX_0x88_FORMAT(list->loc[i]));
    }
    fputs("\n", stdout);
}

void max_pieces_print(max_pieces_t *pieces) {
    max_loclist_print("P", (max_loclist_t*)(&pieces->pawn));
    max_loclist_print("N", (max_loclist_t*)(&pieces->knight));
    max_loclist_print("B", (max_loclist_t*)(&pieces->bishop));
    max_loclist_print("R", (max_loclist_t*)(&pieces->rook));
    max_loclist_print("Q", (max_loclist_t*)(&pieces->queen));
    max_loclist_print("K", (max_loclist_t*)(&pieces->king));
}

#endif

#ifdef MAX_TESTS

#include "private/test.h"
#include "max/board/squares.h"

void max_pieces_unit_tests(void) {
    max_pieces_t pieces;
    max_pieces_new(&pieces);
    
    max_loclist_t *list = max_pieces_get_list(&pieces, max_piececode_new(MAX_PIECECODE_WHITE, MAX_PIECECODE_PAWN));
    ASSERT(list->len == 0, "List not initialized correctly");

    max_loclist_add(list, MAX_E4);
    ASSERT(list->len == 1, "Adding location does not update length");
    ASSERT(list->loc[0].v == MAX_E4.v, "Adding location does not update buffer");

    max_loclist_remove(list, 0);
    ASSERT(list->len == 0, "Removing location from list does not update length");
    max_loclist_add(list, MAX_E2);
    ASSERT(list->loc[0].v == MAX_E2.v, "Adding location in removed slot does not update buffer");
}

#endif
