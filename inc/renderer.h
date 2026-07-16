#ifndef SUSTAIN_RENDERER_H
#define SUSTAIN_RENDERER_H

#include "raylib.h"
struct CoreContext;

typedef struct {
  Camera3D camera;
  RenderTexture2D viewport_editor;
} RendererContext;

void renderer_context_init(struct CoreContext *ctx);

#endif // SUSTAIN_RENDERER_H
