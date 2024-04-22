#include "max/engine/tt.h"


void max_ttbl_new(max_ttbl_t *tbl) {
    for(unsigned i = 0; i < MAX_TTBL_LEN; ++i) {
        tbl->array[i].attr.gravestone = true;
    } 
}

max_ttentry_t* max_ttbl_probe(max_ttbl_t *tbl, max_zobrist_t hash) {
    max_ttentry_t *entry = &tbl->array[max_ttbl_extract_index(hash)];
    if(entry->attr.gravestone || entry->key_part != max_ttbl_extract_key(hash)) {
        return NULL;
    } else {
        return entry;
    }
}

max_ttentry_t* max_ttbl_slot(max_ttbl_t *tbl, max_zobrist_t hash, uint8_t ply, uint8_t depth) {
    if(depth <= 3) {
        return NULL;
    }

    max_ttentry_t *slot = &tbl->array[max_ttbl_extract_index(hash)];
    if(!slot->attr.gravestone) {
        if(slot->attr.age - ply >= 3 || slot->attr.depth < depth) {
            return slot;
        } else {
            return NULL;
        }
    }

    return slot;
}
