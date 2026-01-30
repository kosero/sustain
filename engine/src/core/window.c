#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int SN_Init_Window(const int width, const int height, const char *title,
                   bool vsync) {
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

  SDL_Log("INFO: Window and Renderer initialized correctly.");
  return 0;
}

void SN_Window_Close(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

SDL_Window *SN_Get_Window(void) { return window; }
SDL_Renderer *SN_Get_Renderer(void) { return renderer; }
