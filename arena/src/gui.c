#include "gui.h"
#include "max/piece.h"
#include "max/square.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_render.h>
#include <SDL_video.h>

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
        SDL_WINDOW_OPENGL
    );
    
    state->render = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);

    if(state->render == NULL) {
        printf("Failed to create accelerated renderer: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_SetRenderDrawColor(state->render, 0xFF, 0, 0xFF, 0xFF);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    gui_textures_load(state->render, &state->textures);

    max_engine_new(&state->engine);

    return 0;
}

static max_bpos_t screen_to_board(gui_state_t *state, int x, int y) {
    int w;
    int h;
    SDL_GetWindowSize(state->window, &w, &h);

    int x_step = w / 8;
    int y_step = h / 8;

    return max_bpos_new(x / x_step, 7 - (y / y_step));
}

static SDL_Texture* gui_texture_for_piece(gui_state_t *state, max_piececode_t piece) {
    SDL_Texture *tx = NULL;
    if(piece & MAX_PIECECODE_WHITE) {
        tx = state->textures.white[piece & MAX_PIECECODE_TYPE_MASK];
    } else {
        tx = state->textures.black[piece & MAX_PIECECODE_TYPE_MASK];
    }
    return tx;
}

static void gui_state_render(gui_state_t *state) {
    SDL_Rect dest;
    dest.w = state->squarex;
    dest.h = state->squarey;

    for(unsigned x = 0; x < 8; ++x) {
        for(unsigned y = 0; y < 8; ++y) {
            dest.x = x * state->squarex;
            dest.y = (7 - y) * state->squarey;

            max_bpos_t pos = max_bpos_new(x, y);
            SDL_Texture *bg = NULL;
            if((x + y) & 1) {
                bg = state->textures.square[0];
            } else {
                bg = state->textures.square[1];
            }
            SDL_RenderCopy(state->render, bg, NULL, &dest);

            max_piececode_t piece = state->engine.board.pieces[pos];
            if((state->grabbed.grabbed == NULL || pos != state->grabbed.from) && piece != MAX_PIECECODE_EMPTY) {
                SDL_RenderCopy(state->render, gui_texture_for_piece(state, piece), NULL, &dest);
            }
        }
    }
}

int gui_state_run(gui_state_t *state) {
    int w;
    int h;
    
    SDL_GetWindowSize(state->window, &w, &h);

    state->squarex = w / 8;
    state->squarey= h / 8;

    for(;;) {
        SDL_Event event;
        SDL_RenderClear(state->render);
        gui_state_render(state);
        if(state->grabbed.grabbed != NULL) {
            SDL_Rect pos;
            SDL_GetMouseState(&pos.x, &pos.y);
            pos.w = state->squarex;
            pos.h = state->squarey;
            pos.x -= pos.w / 2;
            pos.y -= pos.h / 2;
            SDL_RenderCopy(state->render, state->grabbed.grabbed, NULL, &pos);
        }

        SDL_RenderPresent(state->render);


        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: return 0;
                case SDL_MOUSEBUTTONUP: {
                    if(state->grabbed.grabbed != NULL) {
                        max_bpos_t to = screen_to_board(state, event.button.x, event.button.y);
                        if(max_bpos_valid(to)) {
                        }
                    } else {
                        max_bpos_t pos = screen_to_board(state, event.button.x, event.button.y);
                        if(max_bpos_valid(pos)) {
                            max_piececode_t piece = state->engine.board.pieces[pos];
                            if(piece != MAX_PIECECODE_EMPTY) {
                                state->grabbed.from = pos;
                                state->grabbed.grabbed = gui_texture_for_piece(state, piece);
                                SDL_ShowCursor(SDL_DISABLE);
                            }
                        }
                    }
                } break;
            }
        }

        SDL_Delay(16);
    }
}

void gui_state_destroy(gui_state_t *state) {
    if(state->render != NULL) {
        SDL_DestroyRenderer(state->render);
    }

    if(state->window != NULL) {
        SDL_DestroyWindow(state->window);
    }

    SDL_Quit();
}
