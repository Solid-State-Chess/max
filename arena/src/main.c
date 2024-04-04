#include "gui.h"
#include <stdlib.h>

#include <SDL2/SDL.h>

int main(void) {
    int ec;
    gui_state_t *gui = malloc(sizeof(*gui)); 

    if((ec = gui_state_new(gui)) < 0) {
        goto gui_fail;
    }

    if((ec = gui_state_run(gui)) < 0) {
        goto gui_fail;
    }

gui_fail:
    gui_state_destroy(gui);
    free(gui);

    return 1;
}
