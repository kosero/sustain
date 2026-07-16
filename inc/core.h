#ifndef SUSTAIN_CORE_H
#define SUSTAIN_CORE_H

#include "renderer.h"

typedef struct CoreContext {
  RendererContext renderer;
} CoreContext;

CoreContext *get_core_context(void);
void set_core_context(CoreContext *ctx);
void core_context_init(void);

void core_run(void);

#endif // SUSTAIN_CORE_H
