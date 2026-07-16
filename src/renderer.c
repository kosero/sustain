#include "renderer.h"
#include "core.h"
#include "raylib.h"

void renderer_context_init(CoreContext *ctx) {
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 10.0f, 10.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 80.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  ctx->renderer.camera = camera;

  ctx->renderer.viewport_editor = LoadRenderTexture(800, 600);
}
