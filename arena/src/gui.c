#include "gui.h"
#include "max/board/board.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/engine/engine.h"
#include "max/engine/score.h"
#include "max/engine/tt.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_mutex.h>
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
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    gui_textures_load(state->render, &state->textures);
    
    state->shared = malloc(sizeof(*state->shared));

    static const unsigned BOARD_STACK_CAP  = 100;
    static const unsigned MOVELIST_CAP     = 70 * MAX_ENGINE_MAX_MOVES_PER_PLY;
    static const unsigned TTBL_BUF_CAP_BIT = 23;
    static const unsigned TTBL_BUF_CAP     = (1 << TTBL_BUF_CAP_BIT);
    
    max_engine_init_params_t init = (max_engine_init_params_t){
        .board = {
            .stack = malloc(sizeof(max_state_t) * BOARD_STACK_CAP),
            .capacity = BOARD_STACK_CAP
        },
        .ttbl = {
            .buf = malloc(sizeof(max_ttentry_t) * TTBL_BUF_CAP),
            .nbit = TTBL_BUF_CAP_BIT
        },
        .moves = {
            .buf = malloc(sizeof(max_smove_t) * MOVELIST_CAP),
            .capacity = MOVELIST_CAP
        }
    };

    max_engine_new(&state->shared->engine, &init, max_eval_params_default());
    max_board_default_pos(&state->shared->engine.board);
    max_movelist_new(&state->shared->moves, malloc(sizeof(max_smove_t) * 128), 128);

    state->shared->quit = false;
    state->shared->lock = SDL_CreateSemaphore(0);

    state->thread = SDL_CreateThread(gui_engine_thread, "Engine Thread", (void*)state->shared);
    if(state->thread == NULL) {
        printf("Failed to create engine thread: %s\n", SDL_GetError());
        return -1;
    }

    state->grabbed.grabbed = NULL;

    state->promote.selecting = false;
    state->promote.selected = 0;

    return 0;
}

static max_0x88_t screen_to_board(gui_state_t *state, int x, int y) {
    int w;
    int h;
    SDL_GetWindowSize(state->window, &w, &h);

    int x_step = w / 8;
    int y_step = h / 8;

    return max_0x88_new(7 - (y / y_step), x / x_step);
}

static SDL_Texture* gui_texture_for_piece(gui_state_t *state, max_piececode_t piece) {
    SDL_Texture *tx = NULL;
    if(piece.v & MAX_PIECECODE_WHITE) {
        tx = state->textures.white[piece.v & MAX_PIECECODE_TYPE_MASK];
    } else {
        tx = state->textures.black[piece.v & MAX_PIECECODE_TYPE_MASK];
    }
    return tx;
}

/// MUST have lock
static void gui_state_render(gui_state_t *state) {
    SDL_Rect dest;
    dest.w = state->squarex;
    dest.h = state->squarey;

    for(unsigned x = 0; x < 8; ++x) {
        for(unsigned y = 0; y < 8; ++y) {
            dest.x = x * state->squarex;
            dest.y = (7 - y) * state->squarey;

            max_0x88_t pos = max_0x88_new(y, x);
            SDL_Texture *bg = NULL;
            if((x + y) & 1) {
                bg = state->textures.square[0];
            } else {
                bg = state->textures.square[1];
            }
            SDL_RenderCopy(state->render, bg, NULL, &dest);

            max_piececode_t piece = state->shared->engine.board.pieces[pos.v];
            if(
                (state->grabbed.grabbed == NULL ||pos.v != state->grabbed.from.v) &&
                piece.v != MAX_PIECECODE_EMPTY &&
                (!state->promote.selecting || (max_0x88_file(state->promote.promote_sq) != x || y > 4))
            ) {
                SDL_RenderCopy(state->render, gui_texture_for_piece(state, piece), NULL, &dest);
            }
        }
    }

    if(state->promote.selecting) {
        max_piececode_t promotes[4] = {
            MAX_PIECECODE_QUEEN,
            MAX_PIECECODE_ROOK,
            MAX_PIECECODE_BISHOP,
            MAX_PIECECODE_KNIGHT,
        };

        for(unsigned i = 0; i < 4; ++i) {
            dest.x = max_0x88_file(state->promote.promote_sq) * state->squarex;
            dest.y = i * state->squarey;
            SDL_RenderCopy(
                state->render,
                gui_texture_for_piece(state, max_piececode_new(max_board_side(&state->shared->engine.board), promotes[i].v)),
                NULL,
                &dest
            );
        }
    }
}


static void gui_state_drop_grabbed(gui_state_t *state) {
    state->grabbed.grabbed = NULL;
    SDL_ShowCursor(SDL_ENABLE);
}

int gui_state_run(gui_state_t *state) {
    int w;
    int h;
    
    SDL_GetWindowSize(state->window, &w, &h);

    state->squarex = w / 8;
    state->squarey= h / 8;

    volatile bool enginedone = false;
    SDL_SemPost(state->shared->lock);

    for(;;) {
    outer:
        enginedone = state->shared->done;
        
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
                    if(state->grabbed.grabbed != NULL || state->promote.selecting) {
                        max_0x88_t to = screen_to_board(state, event.button.x, event.button.y);
                        if(state->promote.selecting) {
                            uint8_t selection = max_0x88_rank(to);
                            switch(selection) {
                                case 7: state->promote.selected = MAX_MOVETAG_PQUEEN; break;
                                case 6: state->promote.selected = MAX_MOVETAG_PROOK; break;
                                case 5: state->promote.selected = MAX_MOVETAG_PBISHOP; break;
                                case 4: state->promote.selected = MAX_MOVETAG_PKNIGHT; break;
                                default: continue;
                            }

                            to = state->promote.promote_sq;
                        }

                        if(max_0x88_valid(to)) {
                            if(to.v == state->grabbed.from.v) {
                                gui_state_drop_grabbed(state);
                                continue;
                            }
                            
                            if(enginedone) {
                                max_movelist_t moves = state->shared->moves;
                                for(unsigned i = 0; i < moves.len; ++i) {
                                    max_smove_t move = moves.buf[i];
                                    if(!max_board_legal(&state->shared->engine.board, move)) {
                                        continue;
                                    }

                                    if(move.from.v == state->grabbed.from.v && move.to.v == to.v) {
                                        if(max_movetag_is_promote(move.tag)) {
                                            if(!state->promote.selecting) {
                                                state->promote.selecting = true;
                                                state->promote.selected = 0;
                                                state->promote.promote_sq = move.to;
                                                goto outer;
                                            } else if(state->promote.selected != 0) {
                                                move.tag = (move.tag & MAX_MOVETAG_CAPTURE) | state->promote.selected;
                                                state->promote.selecting = false;
                                            } else {
                                                goto outer;
                                            }
                                        }

                                        max_board_make_move(&state->shared->engine.board, move);
                                        gui_state_drop_grabbed(state);
                                        enginedone = false;
                                        SDL_SemPost(state->shared->lock);
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        max_0x88_t pos = screen_to_board(state, event.button.x, event.button.y);

                        if(enginedone) {
                            if(max_0x88_valid(pos)) {
                                max_piececode_t piece = state->shared->engine.board.pieces[pos.v];
                                if(piece.v != MAX_PIECECODE_EMPTY) {
                                    state->grabbed.from = pos;
                                    state->grabbed.grabbed = gui_texture_for_piece(state, piece);
                                    SDL_ShowCursor(SDL_DISABLE);
                                }
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
    state->shared->quit = true;
    SDL_SemPost(state->shared->lock);

    int status;
    SDL_WaitThread(state->thread, &status);

    free(state->shared->moves.buf);

    if(state->render != NULL) {
        SDL_DestroyRenderer(state->render);
    }

    if(state->window != NULL) {
        SDL_DestroyWindow(state->window);
    }

    SDL_Quit();
}
