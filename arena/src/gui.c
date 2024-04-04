#include "gui.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_surface.h>

int gui_state_new(gui_state_t *state) {
    int ec;
    if((ec = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) < 0) {
        printf("Failed to initialize SDL2 [%d]: %s\n", ec, SDL_GetError());
        return ec;
    }

    state->window = SDL_CreateWindow(
        "Max Chess Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 800,
        0
    );
    
    state->surface = SDL_GetWindowSurface(state->window);

    return 0;
}

int gui_state_run(gui_state_t *state) {
    SDL_Event event;
    for(;;) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: return 0;
            }
        }

        SDL_UpdateWindowSurface(state->window);
    }
}

void gui_state_destroy(gui_state_t *state) {
    if(state->window != NULL) {
        SDL_DestroyWindow(state->window);
    }

    SDL_Quit();
}
