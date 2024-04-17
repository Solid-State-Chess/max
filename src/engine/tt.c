#include "max/engine/tt.h"
#include "max/def.h"


void max_ttbl_new(max_ttbl_t *tbl) {
    for(unsigned i = 0; i < MAX_TTBL_LEN; ++i) {
        tbl->array[i].attr.gravestone = true;
    } 
}

MAX_INLINE_ALWAYS
static uint32_t max_extract_tbl_index(max_zobrist_t hash) {
    static const max_zobrist_t MASK = (1 << MAX_TTBL_INDEX_BITS) - 1;
    return hash & MASK;
}

MAX_INLINE_ALWAYS
static max_zobrist_key_t max_extract_ttbl_key(max_zobrist_t hash) {
    return hash >> MAX_TTBL_INDEX_BITS;
}

max_ttentry_t* max_ttbl_probe(max_ttbl_t *tbl, max_zobrist_t hash) {
    max_ttentry_t *entry = &tbl->array[max_extract_tbl_index(hash)];
    if(entry->attr.gravestone || entry->key_part != max_extract_ttbl_key(hash)) {
        return NULL;
    } else {
        return entry;
    }
}

void max_ttbl_insert(max_ttbl_t *tbl, max_zobrist_t hash, max_ttentry_t entry) {
    max_ttentry_t *slot = &tbl->array[max_extract_tbl_index(hash)];
    if(!slot->attr.gravestone) {
        if(slot->attr.depth > entry.attr.depth) {
            return;
        }
    }
    *slot = entry;
}

max_ttentry_t max_ttentry_pv(max_zobrist_t pos, max_score_t score, max_move_t move, uint8_t depth) {
    return (max_ttentry_t){
        .key_part = max_extract_ttbl_key(pos),
        .move = move,
        .score = score,
        .attr = (max_ttentry_attr_t){
            .depth = depth,
            .gravestone = false,
            .node_type = MAX_TTENTRY_TYPE_PV
        }
    };
}

max_ttentry_t max_ttentry_betacutoff(max_zobrist_t pos, max_score_t score, max_move_t move, uint8_t depth) {
    return (max_ttentry_t){
        .key_part = max_extract_ttbl_key(pos),
        .move = move,
        .score = score,
        .attr = (max_ttentry_attr_t){
            .depth = depth,
            .gravestone = false,
            .node_type = MAX_TTENTRY_TYPE_LOWER
        }
    };
}

max_ttentry_t max_ttentry_all(max_zobrist_t pos, max_score_t score, max_move_t move, uint8_t depth) {
    return (max_ttentry_t){
        .key_part = max_extract_ttbl_key(pos),
        .move = move,
        .score = score,
        .attr = (max_ttentry_attr_t){
            .depth = depth,
            .gravestone = false,
            .node_type = MAX_TTENTRY_TYPE_UPPER
        }
    };
}
