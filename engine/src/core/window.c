#include "sustain/core/log.h"
#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int SN_Init_Window(const int width, const int height, const char *title,
                   bool vsync, const char *backend) {

  if (backend && strcmp(backend, "auto") != 0) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, backend);
    SLOG_INFO("[INIT] Requested renderer backend: %s", backend);
  } else {
    SLOG_WARN("[INIT] Renderer backend: auto");
  }

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SLOG_ERROR("ERROR: Failed to initialize SDL video subsystem: %s",
            SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow(title, width, height, 0);
  if (window == NULL) {
    SLOG_ERROR("ERROR: Failed to create window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  renderer = SDL_CreateRenderer(window, NULL);
  if (renderer == NULL) {
    SLOG_ERROR("ERROR: Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_SetRenderVSync(renderer, vsync);

  SLOG_INFO("[INFO]: Window initialized | Title=\"%s\" | Size=%dx%d | VSync=%s | "
          "Renderer=\"%s\" | Driver=\"%s\"",
          title, width, height, vsync ? "ON" : "OFF",
          SDL_GetRendererName(renderer), SDL_GetCurrentVideoDriver());

  return 0;
}

void SN_Window_Close(void) {
  SLOG_INFO("[SHUTDOWN]: Destroying renderer...");
  SDL_DestroyRenderer(renderer);

  SLOG_INFO("[SHUTDOWN]: Destroying window...");
  SDL_DestroyWindow(window);

  SLOG_INFO("[SHUTDOWN]: Shutting down SDL subsystems...");
  SDL_Quit();

  SLOG_INFO("[SHUTDOWN]: Window system shut down successfully.");
}

SDL_Window *SN_Get_Window(void) { return window; }
SDL_Renderer *SN_Get_Renderer(void) { return renderer; }
