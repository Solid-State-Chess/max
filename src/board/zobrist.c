#include "private/board/zobrist.h"
#include "max/board/piececode.h"
#include <stdint.h>

// xorshift256** general purpose random number generator
struct max_zobrist_xoshiro256 {
    uint64_t v[4];
};

static uint64_t max_zobrist_rol64(uint64_t v, unsigned k) {
    return (v << k) | (v >> (64 - k));
}

static max_zobrist_t max_zobrist_rng(struct max_zobrist_xoshiro256 *state) {
    uint64_t *v = state->v;
    const uint64_t result = max_zobrist_rol64(v[1] * 5, 7) * 9;
    const uint64_t t = v[1] << 17;
    v[2] ^= v[0];
    v[3] ^= v[1];
    v[1] ^= v[2];
    v[0] ^= v[3];

    v[2] ^= t;
    v[3] ^= max_zobrist_rol64(v[3], 45);

    return (max_zobrist_t)result;
}


static uint64_t splitmix_64(uint64_t *state) {
    uint64_t result = (*state += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}


// Initialize a random number generator by mixing a seed
static void max_zobrist_init_xoshiro(struct max_zobrist_xoshiro256 *state, uint64_t seed) {
    state->v[0] = splitmix_64(&seed);
    state->v[1] = splitmix_64(&seed);
    state->v[2] = splitmix_64(&seed);
    state->v[3] = splitmix_64(&seed);
}

void max_zobrist_elements_init(max_zobrist_elements_t *elems, uint64_t seed) {
    struct max_zobrist_xoshiro256 state;
    max_zobrist_init_xoshiro(&state, seed);

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
