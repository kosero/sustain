#include "sustain.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "sdl_context.h"

int init_window(uint32_t w, uint32_t h, const char *title)
{
	sdl_context_init(title, (int)w, (int)h, 0);

	return 0;
}

void close_window(void) { SDL_Quit(); }

int window_quit(void)
{
	SDL_Event e;

	SDL_WaitEvent(&e);

	if (e.type == SDL_EVENT_QUIT) {
		return 1;
	}

	return 0;
}

void frame_clear(Color color)
{
	SDL_SetRenderDrawColor(get_sdl_context()->renderer, color.r, color.g,
			       color.b, color.a);
	SDL_RenderClear(get_sdl_context()->renderer);
}

void next_frame(void) { SDL_RenderPresent(get_sdl_context()->renderer); }
