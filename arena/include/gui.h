#pragma once
#include "max/engine.h"
#include <SDL2/SDL.h>

typedef struct {
    /// White and black textures
    SDL_Texture *square[2];
    /// Lookup table for piece codes 
    SDL_Texture *white[32];
    /// Lookup table for black piece codes
    SDL_Texture *black[32];
} gui_textures_t;

/// State for the currently grabbed piece
typedef struct {
    /// If NULL, then there is no piece being grabbed
    SDL_Texture *grabbed;
    max_bpos_t from;
} gui_grabbed_t;

/// State shared between engine thread and the gui thread
typedef struct {
    /// Engine state including the game board
    max_engine_t engine;
    /// A list of all valid moves for the player, filled by the engine thread
    max_movelist_t moves;
    /// Lock preventing simultaneous acccess to the board
    SDL_sem *lock;
    /// Set while the GUI has a lock indicating that the engine thread should exit
    bool quit;
} gui_shared_t;

/// State for pawn promotion
typedef struct {
    /// If the player is currently selecting a piece to promote to
    bool selecting;
    /// Square to promote at
    max_bpos_t promote_sq;
    /// Template to fill with promotion piece
    max_move_attr_t selected;
} gui_promote_t;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *render;
    /// Thread that the engine runs in
    SDL_Thread *thread;
    /// State that the engine thread has access to
    gui_shared_t *shared;

    gui_textures_t textures;

    gui_grabbed_t grabbed;
    //Size of each chess square in pixels
    int squarex;
    int squarey;

    gui_promote_t promote;
} gui_state_t;

/// Initialize the window and game engine
int gui_state_new(gui_state_t *state);

/// Run the GUI until the user exits the program
int gui_state_run(gui_state_t *state);

/// Destroy this GUI state, closing all windows but NOT freeing the memory for gui state
void gui_state_destroy(gui_state_t *state);

/// Load all textures from their embedded binary
int gui_textures_load(SDL_Renderer *render, gui_textures_t *texture);

/// Function to start as a thread in SDL
int gui_engine_thread(void* data);
