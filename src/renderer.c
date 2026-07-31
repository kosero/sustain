#include "renderer.h"
#include "core.h"
#include "raylib.h"
#include "rlgl.h"

void renderer_context_init(struct CoreContext *ctx) {
  RendererContext *rctx = &ctx->renderer;

  rctx->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  rctx->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  rctx->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  rctx->camera.fovy = 60.0f;
  rctx->camera.projection = CAMERA_PERSPECTIVE;

  int initial_w = 640;
  int initial_h = 480;
  rctx->viewport_rt = LoadRenderTexture(initial_w, initial_h);
  rctx->viewport_width = initial_w;
  rctx->viewport_height = initial_h;
  rctx->viewport_initialized = true;
}

void renderer_viewport_resize(RendererContext *rctx, int width, int height) {
  if (width < 1) {
    width = 1;
  }
  if (height < 1) {
    height = 1;
  }

  if (rctx->viewport_width == width && rctx->viewport_height == height) {
    return;
  }

  if (rctx->viewport_initialized) {
    UnloadRenderTexture(rctx->viewport_rt);
  }

  rctx->viewport_rt = LoadRenderTexture(width, height);
  rctx->viewport_width = width;
  rctx->viewport_height = height;
  rctx->viewport_initialized = true;
}

void renderer_draw_viewport(RendererContext *rctx) {
  BeginTextureMode(rctx->viewport_rt);
  ClearBackground((Color){30, 30, 35, 255});

  BeginMode3D(rctx->camera);

  DrawGrid(20, 1.0f);

  DrawCube((Vector3){0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f,
           (Color){80, 140, 220, 255});
  DrawCubeWires((Vector3){0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f,
                (Color){50, 100, 180, 255});

  DrawSphere((Vector3){4.0f, 1.0f, 0.0f}, 1.0f, (Color){220, 80, 80, 255});

  DrawLine3D((Vector3){0}, (Vector3){3, 0, 0}, RED);
  DrawLine3D((Vector3){0}, (Vector3){0, 3, 0}, GREEN);
  DrawLine3D((Vector3){0}, (Vector3){0, 0, 3}, BLUE);

  EndMode3D();
  EndTextureMode();
}

void renderer_context_cleanup(RendererContext *rctx) {
  if (rctx->viewport_initialized) {
    UnloadRenderTexture(rctx->viewport_rt);
    rctx->viewport_initialized = false;
  }
}
