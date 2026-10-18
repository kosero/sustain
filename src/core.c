#include "SDL3/SDL_init.h"
#include "dev/rhi/rhi.h"
#include "sustain.h"

int init_window(uint32_t w, uint32_t h, const char *title)
{
	return rhi_init(BACKEND_OPENGL, title, (int)w, (int)h);
}

void close_window(void) { SDL_Quit(); }

void frame_clear(Color color) { rhi()->clear(color); }

void next_frame(void) { rhi()->present(); }
