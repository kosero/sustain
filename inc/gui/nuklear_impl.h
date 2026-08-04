#ifndef SUSTAIN_NUKLEAR_IMPL_H
#define SUSTAIN_NUKLEAR_IMPL_H

#include "SDL3/SDL.h"
struct nk_context;

struct nk_context *nuklear_init(int font_size);
void nuklear_shutdown(struct nk_context *ctx);
void nuklear_handle_event(struct nk_context *ctx, const SDL_Event *evt);
void nuklear_render(struct nk_context *ctx, int width, int height);

#endif // SUSTAIN_NUKLEAR_IMPL_H
