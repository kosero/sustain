#ifndef SUSTAIN_RENDERER_H
#define SUSTAIN_RENDERER_H

#include "raylib.h"
struct CoreContext;

typedef struct {
  Camera3D camera;
  RenderTexture2D viewport_rt;
  int viewport_width;
  int viewport_height;
  bool viewport_initialized;
  Rectangle viewport_draw_rect;
} RendererContext;

void renderer_context_init(struct CoreContext *ctx);
void renderer_viewport_resize(RendererContext *rctx, int width, int height);
void renderer_draw_viewport(RendererContext *rctx);
void renderer_context_cleanup(RendererContext *rctx);

#endif // SUSTAIN_RENDERER_H
