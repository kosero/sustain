#ifndef SUSTAIN_MICROUI_IMPL_H
#define SUSTAIN_MICROUI_IMPL_H

#include "SDL3/SDL_events.h"
struct mu_Context;

struct mu_Context *microui_init(int font_size);
void microui_shutdown(struct mu_Context *ctx);
void microui_handle_event(struct mu_Context *ctx, const SDL_Event *evt);
void microui_update_text_input(struct mu_Context *ctx);
void microui_render(struct mu_Context *ctx, int width, int height);

#endif // SUSTAIN_MICROUI_IMPL_H
