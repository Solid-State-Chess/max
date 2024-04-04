#pragma once
#include <SDL2/SDL.h>
#include <SDL_video.h>

typedef struct {
    SDL_Window *window;
    SDL_Surface *surface;
} gui_state_t;

/// Initialize the window and game engine
int gui_state_new(gui_state_t *state);

/// Run the GUI until the user exits the program
int gui_state_run(gui_state_t *state);

/// Destroy this GUI state, closing all windows but NOT freeing the memory for gui state
void gui_state_destroy(gui_state_t *state);
