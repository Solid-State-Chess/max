#include "gui.h"
#include <SDL2/SDL_image.h>

#include "img/k.h"
#include "img/q.h"
#include "img/r.h"
#include "img/b.h"
#include "img/n.h"
#include "img/p.h"


#include "img/K.h"
#include "img/Q.h"
#include "img/R.h"
#include "img/B.h"
#include "img/N.h"
#include "img/P.h"
#include "max/piece.h"
#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
#include <stdint.h>

static SDL_Texture* load_texture(SDL_Renderer *render, uint8_t *buf, size_t len) {
    SDL_Texture *tx = NULL;
    
    SDL_RWops *ops = SDL_RWFromConstMem(buf, (int)len);
    SDL_Surface *surf = IMG_LoadPNG_RW(ops);
    if(surf == NULL) {
        printf("Failed to load embedded PNG image: %s\n", IMG_GetError());
        return NULL;
    }

    tx = SDL_CreateTextureFromSurface(render, surf);
    if(tx == NULL) {
        printf("Failed to convert embedded PNG surface to texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surf);
        return NULL;
    }

    SDL_FreeSurface(surf);
    return tx;
}

int gui_textures_load(SDL_Renderer *render, gui_textures_t *textures) {
    textures->white[MAX_PIECECODE_PAWN]   = load_texture(render, __p_png, __p_png_len);
    textures->white[MAX_PIECECODE_KNIGHT] = load_texture(render, __n_png, __n_png_len);
    textures->white[MAX_PIECECODE_BISHOP] = load_texture(render, __b_png, __b_png_len);
    textures->white[MAX_PIECECODE_ROOK]   = load_texture(render, __r_png, __r_png_len);
    textures->white[MAX_PIECECODE_QUEEN]  = load_texture(render, __q_png, __q_png_len);
    textures->white[MAX_PIECECODE_KING]   = load_texture(render, __k_png, __k_png_len);

    textures->black[MAX_PIECECODE_PAWN]   = load_texture(render, __P_png, __P_png_len);
    textures->black[MAX_PIECECODE_KNIGHT] = load_texture(render, __N_png, __N_png_len);
    textures->black[MAX_PIECECODE_BISHOP] = load_texture(render, __B_png, __B_png_len);
    textures->black[MAX_PIECECODE_ROOK]   = load_texture(render, __R_png, __R_png_len);
    textures->black[MAX_PIECECODE_QUEEN]  = load_texture(render, __Q_png, __Q_png_len);
    textures->black[MAX_PIECECODE_KING]   = load_texture(render, __K_png, __K_png_len);

    return 0;
}
