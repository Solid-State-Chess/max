#pragma once

#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/state.h"


/// \ingroup state
/// @{

/// \name Private Functions
/// @{

/// Create a new check structure that represents no detected check from a square.
MAX_INLINE_ALWAYS max_check_t max_check_empty(void) {
    return (max_check_t){
        .origin = MAX_0x88_INVALID_MASK,
        .ray = MAX_0x88_DIR_INVALID,
    };
}

/// Get a default state plate representing both sides having castle rights,
/// no en passant possible, and neither side in check
MAX_INLINE_ALWAYS max_state_t max_state_default(void) {
    return (max_state_t){
        .position = 0,
        .state = 0xFF,
        .check = {
            max_check_empty(),
            max_check_empty()
        }
    };
}

/// Create a new state stack with a single element from the given buffer
/// \param [out] stack The stack to initialize to length 1
/// \param [in] buf The buffer to use as the backing storage for the stack
/// \param [in] state First state plate to place on the stack
MAX_INLINE_ALWAYS void max_state_stack_new(max_state_stack_t *stack, max_state_t *buf, max_state_t state) {
    *buf = state;
    stack->plates = buf;
    stack->head = 0;
}


/// @}

/// @}
