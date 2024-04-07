#include "gui.h"
#include <stdlib.h>

#include <SDL2/SDL.h>

#ifdef _WIN32
#include <Windows.h>
int __clrcall WinMain(
  HINSTANCE _hInstance,
  HINSTANCE _hPrevInstance,
  LPSTR     _lpCmdLine,
  int       _nShowCmd
) {
#else
int main(void) {
#endif
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
