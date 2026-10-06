#include "sdl_context.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include <stdlib.h>

static SDL_Context *sdl_context(void)
{
	static SDL_Context ctx;
	return &ctx;
}
SDL_Context *get_sdl_context(void) { return sdl_context(); }

void sdl_context_init(const char *title, int w, int h, SDL_WindowFlags flags)
{
	SDL_Context *ctx = get_sdl_context();

	if (!SDL_CreateWindowAndRenderer(title, w, h, flags, &ctx->window,
					 &ctx->renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		exit(1);
	}
}
