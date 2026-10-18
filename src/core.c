#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "dev/sdl_context.h"
#include "sustain.h"

int init_window(uint32_t w, uint32_t h, const char *title)
{
	return sdl_context_init(title, (int)w, (int)h, 0);
}

void close_window(void) { SDL_Quit(); }

void frame_clear(Color color)
{
	SDL_SetRenderDrawColor(get_sdl_context()->renderer, color.r, color.g,
			       color.b, color.a);
	SDL_RenderClear(get_sdl_context()->renderer);
}

void next_frame(void) { SDL_RenderPresent(get_sdl_context()->renderer); }
