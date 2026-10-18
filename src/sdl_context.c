#include "dev/sdl_context.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"

static SDL_Context *sdl_context(void)
{
	static SDL_Context ctx;
	return &ctx;
}
SDL_Context *get_sdl_context(void) { return sdl_context(); }

int sdl_context_init(const char *title, int w, int h, SDL_WindowFlags flags)
{
	SDL_Context *ctx = get_sdl_context();

	ctx->window = SDL_CreateWindow(title, w, h, flags);
	if (ctx->window == NULL) {
		SDL_Log("Couldn't create window: %s", SDL_GetError());
		return -1;
	}

	return 0;
}
