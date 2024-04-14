#include <stdio.h>
#include <string.h>
#define MAX_CONSOLE
#define MAX_DEBUG
#include "max/board/board.h"
#include "max/engine/engine.h"
#include "max.h"
#include "test.h"
#include <stdlib.h>

//#include "valgrind/callgrind.h"

static bool board_same(max_board_t *a, max_board_t *b) {
    if(memcmp(a->pieces, b->pieces, sizeof(a->pieces)) != 0) {
        return false;
    }

    static max_piececode_t PIECES[] = {MAX_PIECECODE_PAWN, MAX_PIECECODE_KNIGHT, MAX_PIECECODE_BISHOP, MAX_PIECECODE_ROOK, MAX_PIECECODE_QUEEN, MAX_PIECECODE_KING};
    for(unsigned i = 0; i <= 1; ++i) {
        for(unsigned j = 0; j < sizeof(PIECES); ++j) {
            max_piecelist_t *al = max_pieces_get_list(&a->sides[i], PIECES[j]);
            max_piecelist_t *bl = max_pieces_get_list(&b->sides[i], PIECES[j]);
            for(unsigned k = 0; k < al->len; ++k) {
                bool match = false;
                for(unsigned l = 0; l < bl->len; ++l) {
                    if(al->pos[k] == bl->pos[l]) { match = true; break; }
                }
                if(!match) {
                    return false;
                }
            }
        }
    }
    
    if(a->captures.len != b->captures.len || memcmp(a->captures.captures, b->captures.captures, a->captures.len) != 0) {
        printf("%u to %u\n", a->captures.len, b->captures.len);
        return false;
    }

    return true;
}

static void printnodes(uint64_t nodes) {
    uint64_t mil = nodes / 1000000;
    printf("%zu.%zu MN", mil, (nodes - (mil * 1000000)) / 100000);
}

int engine_tests(void) {
    max_init_statics();
    max_engine_t *engine = malloc(sizeof(*engine));
    max_engine_new(engine);
    
    max_move_t buf[256];
    max_searchresult_t search;
    /*CALLGRIND_START_INSTRUMENTATION;
    CALLGRIND_TOGGLE_COLLECT;
    //max_engine_search(engine, &search, 6);
    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;*/

    return 0;
}
