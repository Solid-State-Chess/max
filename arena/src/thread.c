#include "gui.h"
#include "max/board/board.h"
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
            double time = (double)(SDL_GetTicks64() - start) / 1000;
            /*double meganodes = (double)(data->engine.diagnostic.nodes) / 1000000;
            double tt_hits = (double)(data->engine.diagnostic.tt_hits);*/

            //double mn_s = meganodes / time;
            printf(
                //"%.3f MN - %.2f s [%.2f MN/s] @ %i (%.0f TT Hits) (depth %d)\n",
                "%c%c%c%c - %d [%.2f s]\n",
                MAX_0x88_FORMAT(search.best.from),
                MAX_0x88_FORMAT(search.best.to),
                search.score,
                time
            );

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
