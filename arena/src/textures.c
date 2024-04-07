#include "gui.h"
#ifdef SDL_IMAGE_VENDORED
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include "img/wk.h"
#include "img/wq.h"
#include "img/wr.h"
#include "img/wb.h"
#include "img/wn.h"
#include "img/wp.h"


#include "img/bk.h"
#include "img/bq.h"
#include "img/br.h"
#include "img/bb.h"
#include "img/bn.h"
#include "img/bp.h"

#include "img/marble.h"
#include "img/wood.h"

#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>

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
    textures->white[MAX_PIECECODE_PAWN]   = load_texture(render, __wp_png, __wp_png_len);
    textures->white[MAX_PIECECODE_KNIGHT] = load_texture(render, __wn_png, __wn_png_len);
    textures->white[MAX_PIECECODE_BISHOP] = load_texture(render, __wb_png, __wb_png_len);
    textures->white[MAX_PIECECODE_ROOK]   = load_texture(render, __wr_png, __wr_png_len);
    textures->white[MAX_PIECECODE_QUEEN]  = load_texture(render, __wq_png, __wq_png_len);
    textures->white[MAX_PIECECODE_KING]   = load_texture(render, __wk_png, __wk_png_len);

    textures->black[MAX_PIECECODE_PAWN]   = load_texture(render, __bp_png, __bp_png_len);
    textures->black[MAX_PIECECODE_KNIGHT] = load_texture(render, __bn_png, __bn_png_len);
    textures->black[MAX_PIECECODE_BISHOP] = load_texture(render, __bb_png, __bb_png_len);
    textures->black[MAX_PIECECODE_ROOK]   = load_texture(render, __br_png, __br_png_len);
    textures->black[MAX_PIECECODE_QUEEN]  = load_texture(render, __bq_png, __bq_png_len);
    textures->black[MAX_PIECECODE_KING]   = load_texture(render, __bk_png, __bk_png_len);

    textures->square[0]                   = load_texture(render, __marble_png, __marble_png_len);
    textures->square[1]                   = load_texture(render, __wood_png,   __wood_png_len);

    return 0;
}
