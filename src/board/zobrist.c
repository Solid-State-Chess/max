#include "private/board/zobrist.h"
#include "max/board/piececode.h"

struct max_zobrist_rng {
    max_zobrist_t x, y, z, w;
};

static max_zobrist_t max_zobrist_rng(struct max_zobrist_rng *state) {
    
}

void max_zobrist_elements_init(max_zobrist_elements_t *elems, max_zobrist_t seed) {
    struct max_zobrist_rng state = {
        .x = seed,
        .y = seed,
        .z = seed,
        .w = seed
    };

    for(unsigned color = 0; color < MAX_SIDES_LEN; ++color) {
        for(unsigned i = 0; i < MAX_PIECEINDEX_LEN; ++i) {
            for(unsigned j = 0; j < MAX_6BIT_LEN; ++j) {
                elems->position[color][i][j] = max_zobrist_rng(&state);
            }
        }
    }

    for(unsigned i = 0; i < 4; ++i) {
        elems->castlerights[i] = max_zobrist_rng(&state);
    }

    for(unsigned i = 0; i < 8; ++i) {
        elems->en_passant_file[i] = max_zobrist_rng(&state);
    }
}
