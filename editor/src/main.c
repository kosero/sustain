#include "sustain/core/window.h"
#include "sustain/renderer/renderer.h"
#include <SDL3/SDL.h>

int main(int argc, char *argv[]) {
  if (SN_Init_Window(800, 600, "Test", true) != 0) {
    return 1;
  }

  SDL_Renderer *renderer = SN_Get_Renderer();
  SDL_Event event;

  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    SN_Clear_Background((SN_Color){28, 28, 28});
    SDL_RenderPresent(renderer);
  }

  SN_Window_Close();
  return 0;
}
