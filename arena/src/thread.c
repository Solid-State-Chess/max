#define MAX_CONSOLE
#include "gui.h"
#include "max/board/board.h"
#include "max/board/movegen.h"
#include <SDL_error.h>
#include <SDL_timer.h>

#define NODEFMT(nodes) ((nodes) / 1000000), (((nodes) - (((nodes) / 1000000) * 1000000)) / 10000)

int gui_engine_thread(void *_data) {
    gui_shared_t *data = (gui_shared_t*)_data;
    max_searchresult_t search;

    for(;;) {
        if(SDL_SemWait(data->lock) == 0) {
            data->done = false;
            if(data->quit) {
                break;
            }
            
            uint64_t start = SDL_GetTicks64();
            if(!max_engine_search(&data->engine, &search)) {
                puts("Bot is in checkmate");
                return 0;
            }
            double time = (double)(SDL_GetTicks64() - start) / 1000;
            double meganodes = (double)(data->engine.diagnostic.nodes) / 1000000;
            double tt_hits = (double)(data->engine.diagnostic.tt_hits);
            
            if(search.bestmove.from != search.bestmove.to) {
                max_board_make_move(&data->engine.board, search.bestmove);
                max_board_debugprint(&data->engine.board);
            }

            double mn_s = meganodes / time;
            printf(
                "%.3f MN - %.2f s [%.2f MN/s] @ %i (%.0f TT Hits) (depth %d)\n",
                meganodes,
                time,
                mn_s,
                search.best_score,
                tt_hits,
                search.depth
            );

            max_movelist_clear(&data->moves);
            max_board_movegen_pseudo(&data->engine.board, &data->moves);
            data->done = true;
        } else {
            printf("Failed to wait semaphore in engine thread: %s\n", SDL_GetError());
            break;
        }
    }

    return 0;
}
