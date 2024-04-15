#include "gui.h"
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
            if(!max_engine_search(&data->engine, &search, 6)) {
                puts("Bot is in checkmate");
                return 0;
            }
            double time = (double)(SDL_GetTicks64() - start) / 1000;
            double meganodes = (double)(data->engine.diagnostic.nodes) / 1000000;
            
            if(search.bestmove.from != search.bestmove.to) {
                max_board_make_move(&data->engine.board, search.bestmove);
            }

            double mn_s = meganodes / time;
            printf(
                "%.3f MN - %.2f s [%.2f MN/s] @ %i\n",
                meganodes,
                time,
                mn_s,
                search.best_score
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
