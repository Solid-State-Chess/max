#pragma once

#include "max/board/state.h"


/// \ingroup state
/// @{

/// \name Private Functions
/// @{

/// Get a default state plate representing both sides having castle rights,
/// no en passant possible, and neither side in check
MAX_INLINE_ALWAYS max_state_t max_state_default(void) {
    return (max_state_t){
        .position = 0,
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
