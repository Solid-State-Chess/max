#include "max/engine.h"
#include <stdlib.h>

#include <SDL2/SDL.h>

int main(void) {
    max_engine_t *engine = malloc(sizeof(*engine));
    max_engine_new(engine);
    
    max_move_t buf[256];

    for(;;) {
        
    }

    return 1;
}
