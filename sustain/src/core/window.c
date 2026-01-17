#include "sustain/core/window.h"
#include <raylib.h>

void window_init(const int width, const int height, const char *title,
                 const int fullscreen) {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(width, height, title);
  if (fullscreen)
    ToggleFullscreen();
}

void window_close(void) { CloseWindow(); }
