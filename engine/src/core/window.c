#include "SDL3/SDL_log.h"
#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int SN_Init_Window(const int width, const int height, const char *title,
                   bool vsync, const char *backend) {

  if (backend && strcmp(backend, "auto") != 0) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, backend);
    SDL_Log("[INIT] Requested renderer backend: %s", backend);
  } else {
    SDL_Log("[INIT] Renderer backend: auto");
  }

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("ERROR: Failed to initialize SDL video subsystem: %s",
            SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow(title, width, height, 0);
  if (window == NULL) {
    SDL_Log("ERROR: Failed to create window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  renderer = SDL_CreateRenderer(window, NULL);
  if (renderer == NULL) {
    SDL_Log("ERROR: Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_SetRenderVSync(renderer, vsync);

  SDL_Log("[INFO]: Window initialized | Title=\"%s\" | Size=%dx%d | VSync=%s | "
          "Renderer=\"%s\" | Driver=\"%s\"",
          title, width, height, vsync ? "ON" : "OFF",
          SDL_GetRendererName(renderer), SDL_GetCurrentVideoDriver());

  return 0;
}

void SN_Window_Close(void) {
  SDL_Log("[SHUTDOWN]: Destroying renderer...");
  SDL_DestroyRenderer(renderer);

  SDL_Log("[SHUTDOWN]: Destroying window...");
  SDL_DestroyWindow(window);

  SDL_Log("[SHUTDOWN]: Shutting down SDL subsystems...");
  SDL_Quit();

  SDL_Log("[SHUTDOWN]: Window system shut down successfully.");
}

SDL_Window *SN_Get_Window(void) { return window; }
SDL_Renderer *SN_Get_Renderer(void) { return renderer; }
