#pragma once

#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/state.h"
#include "max/def.h"
#include <string.h>


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

/// Set the en passant file of the given state to the specified file number.
MAX_INLINE_ALWAYS max_packed_state_t max_packed_state_set_epfile(max_packed_state_t original, uint8_t file) {
    original &= ~MAX_PSTATE_EPFILE_MASK;
    original |= file;
    return original;
}

/// Get a default state plate representing both sides having castle rights,
/// no en passant possible, and neither side in check
MAX_INLINE_ALWAYS max_state_t max_state_default(void) {
    return (max_state_t){
        .position = 0,
        .packed = 0xFF,
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
    stack->head_ptr = stack->plates;
    stack->head = 0;
}

/// Add a new state plate to the given state stack.
/// \param stack The stack to modify
/// \param plate Stack plate that will become the new stack head
MAX_INLINE_ALWAYS void max_state_stack_push(max_state_stack_t *stack, max_state_t plate) {
    stack->head += 1;
    stack->plates[stack->head] = plate;
    stack->head_ptr += 1;
}

/// Pop the top state plate from the given state stack, restoring the head to the lower plate.
/// This function performs no bounds checking - it is assumed that the state stack will never be empty.
MAX_INLINE_ALWAYS void max_state_stack_pop(max_state_stack_t *stack) {
    stack->head -= 1;
    stack->head_ptr -= 1;
}

/// Remove all elements under the top 'count' elements of the given stack,
/// resetting its length to <= count.
/// This is used to reset the state stack to under four elements to ensure it always has capacity for engine search.
/// \param count Number of elements to move from the head of the stack to the bottom
MAX_INLINE_ALWAYS void max_state_stack_lower_head(max_state_stack_t *stack, uint8_t count) {
    if(stack->head >= count) {
        max_state_t *upper_buf = stack->head_ptr - (count - 1);
        memcpy(stack->plates, upper_buf, count);
        stack->head = count - 1;
        stack->head_ptr = upper_buf;
    }
}

/// @}

/// @}
