#ifndef SUSTAIN_CORE_H
#define SUSTAIN_CORE_H

#include "raylib-nuklear.h"
#include "renderer.h"
#include <flecs.h>

typedef struct CoreContext {
  struct nk_context *nk_ctx;
  RendererContext renderer;
  ecs_world_t *world;
} CoreContext;

CoreContext *get_core_context(void);
void set_core_context(CoreContext *ctx);
void core_context_init(void);

void core_run(void);

#endif // SUSTAIN_CORE_H
