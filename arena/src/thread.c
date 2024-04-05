#include "gui.h"
#include <SDL_error.h>

#define NODEFMT(nodes) ((nodes) / 1000000), (((nodes) - (((nodes) / 1000000) * 1000000)) / 100000)

int gui_engine_thread(void *_data) {
    gui_shared_t *data = (gui_shared_t*)_data;
    max_searchresult_t search;

    for(;;) {
        if(SDL_SemWait(data->lock) == 0) {
            if(data->quit) {
                break;
            }
            
            max_engine_search(&data->engine, &search, 4);
            max_board_make_move(&data->engine.board, search.bestmove);
            printf("%zu.%zu MN\n", NODEFMT(data->engine.diagnostic.nodes));

            max_movelist_clear(&data->moves);
            max_board_movegen_pseudo(&data->engine.board, &data->moves);
        } else {
            printf("Failed to wait semaphore in engine thread: %s\n", SDL_GetError());
            break;
        }
    }

    return 0;
}
