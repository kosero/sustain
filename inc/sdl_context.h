#ifndef SUSTAIN_SDL_CONTEXT_H
#define SUSTAIN_SDL_CONTEXT_H

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

typedef struct SDL_Context {
	SDL_Renderer *renderer;
	SDL_Window *window;
} SDL_Context;

SDL_Context *get_sdl_context(void);
void sdl_context_init(const char *title, int w, int h, SDL_WindowFlags flags);

#endif // SUSTAIN_SDL_CONTEXT_H
