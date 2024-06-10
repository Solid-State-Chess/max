#include "max/engine/tt.h"
#include "max/assert.h"
#include "private/engine/tt.h"
#include <stddef.h>
#include <stdio.h>

void max_ttbl_new(max_ttbl_t *tbl, max_ttentry_t *buf, uint8_t nbits) {
    MAX_ASSERT(nbits < 32 && "Cannot create a transposition table with a buffer larger than 4GiB");
    uint32_t capacity = 1 << nbits;
    for(uint32_t i = 0; i < capacity; ++i) {
        buf[i] = max_ttentry_gravestone();
    }

    tbl->buf = buf;
    tbl->nbit = nbits;
}

max_ttentry_t const* max_ttbl_probe_read(max_ttbl_t *tbl, max_zobrist_t hash) {
    max_ttentry_t *entry = &tbl->buf[max_ttbl_get_index(tbl, hash)];

    //No explicit check for gravestone marker - assumed that 
    //no key part will ever be the gravestone key, so this equal check will always fail
    //for empty entries
    if(entry->key == max_ttbl_get_key(tbl, hash)) {
        return entry;
    } else {
        return NULL;
    }
}

void max_ttbl_probe_insert(max_ttbl_t *tbl, max_zobrist_t hash, max_nodescore_t score, uint16_t ply) {
    max_ttentry_t *entry = &tbl->buf[max_ttbl_get_index(tbl, hash)];

    if(entry->key != MAX_TTENTRY_KEY_GRAVESTONE) {
        return;
        uint8_t age = ply - entry->age;
        if(score.depth < (entry->depth + (age >> 1)) && (ply - entry->age) < 6) {
            return;
        }
    }

    entry->key = max_ttbl_get_key(tbl, hash);
    entry->score = score.score;
    entry->depth = score.depth;
    entry->age = ply;
    entry->attr = max_ttentry_pattr_new(
        score.bestmove.from,
        score.bestmove.to,
        score.kind,
        score.depth
    );
}
