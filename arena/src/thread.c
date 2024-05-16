#include "gui.h"
#include "max/board/movegen.h"
#include <SDL_error.h>
#include <SDL_timer.h>

#define NODEFMT(nodes) ((nodes) / 1000000), (((nodes) - (((nodes) / 1000000) * 1000000)) / 10000)

int gui_engine_thread(void *_data) {
    gui_shared_t *data = (gui_shared_t*)_data;
    max_search_result_t search;

    for(;;) {
        if(SDL_SemWait(data->lock) == 0) {
            data->done = false;
            if(data->quit) {
                break;
            }
            
            uint64_t start = SDL_GetTicks64();
            max_engine_search(&data->engine, &search);
            if(search.gameover) {
                printf("checkmate\n");
                return 0;
            }
            double time = (double)(SDL_GetTicks64() - start) / 1000;
            double meganodes = (double)(data->engine.diagnostic.nodes) / 1000000;
            uint64_t tt_hits = data->engine.diagnostic.ttbl_hits;
            uint64_t tt_used = data->engine.diagnostic.ttbl_used;
            double util_rate = (double)(tt_used) / (double)(tt_hits);

            double mn_s = meganodes / time;
            printf(
                "%c%c%c%c @ %d - depth %u [%.2f s][%.2f MN | %.2f MN/s] %zu / %zu TT Used / Hits (%.3f TTUR)\n",
                MAX_0x88_FORMAT(search.best.from),
                MAX_0x88_FORMAT(search.best.to),
                search.score,
                search.depth,
                time,
                meganodes,
                mn_s,
                tt_used,
                tt_hits,
                util_rate
            );
            
            unsigned count = 0;
            for(unsigned i = 0; i < (1 << data->engine.table.nbit); ++i) {
                if(data->engine.table.buf[i].key != 0) {
                    count += 1;
                }
            }

            printf("%u TT entries used\n", count);

            max_board_make_move(&data->engine.board, search.best);

            max_movelist_clear(&data->moves);
            max_board_movegen(&data->engine.board, &data->moves);
            data->done = true;
        } else {
            printf("Failed to wait semaphore in engine thread: %s\n", SDL_GetError());
            break;
        }
    }

    return 0;
}
