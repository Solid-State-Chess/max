/// \file tt.h
#pragma once
#include "max/def.h"
#include "max/engine/tt.h"

/// \ingroup tt
/// @{

/// \name Private Functions
/// @{

/// A gravestone marker for nonexistent transposition table entry keys indicating that there is
/// no entry at the given slot.
/// Hopefull this key will never actually be created by the zobrist hash function.
#define MAX_TTENTRY_KEY_GRAVESTONE (0)

/// Get a transposition table entry with a marker that the entry has no valid evaluation yet.
MAX_INLINE_ALWAYS max_ttentry_t max_ttentry_gravestone(void) {
    return (max_ttentry_t){
        .key = MAX_TTENTRY_KEY_GRAVESTONE,
    };
}

/// Initialize the given transposition table with the given buffer and bit capacity.
void max_ttbl_new(max_ttbl_t *tbl, max_ttentry_t *buf, uint8_t nbits);


/// @}

/// @}
