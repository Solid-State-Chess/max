#pragma once
#include <SDL2/SDL.h>
#include <SDL_video.h>

typedef struct {
    /// White and black textures
    SDL_Texture *square[2];
    /// Lookup table for piece codes 
    SDL_Texture *white[32];
    /// Lookup table for black piece codes
    SDL_Texture *black[32];
} gui_textures_t;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *render;
    gui_textures_t textures;
} gui_state_t;

/// Initialize the window and game engine
int gui_state_new(gui_state_t *state);

/// Run the GUI until the user exits the program
int gui_state_run(gui_state_t *state);

/// Destroy this GUI state, closing all windows but NOT freeing the memory for gui state
void gui_state_destroy(gui_state_t *state);

/// Load all textures from their embedded binary
int gui_textures_load(gui_textures_t *texture);