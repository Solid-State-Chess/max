#include "max/board/state.h"
#include <string.h>


void max_irreversible_stack_reset(max_irreversible_stack_t *stack) {
    uint8_t plates = (stack->head > 1) ? 2 : stack->head + 1;
    memmove(stack->array, &stack->array[stack->head - (plates - 1)], plates * sizeof(*stack->array));
    stack->head = plates - 1;
}
