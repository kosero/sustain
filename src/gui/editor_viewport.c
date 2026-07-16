#include "gui/editor_viewport.h"
#include "config.h"
#include "core.h"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"

void editor_viewport_panel(CoreContext *ctx) {
  editor_viewport_render(ctx);

  WindowProperty *w = get_window_property();
  Rectangle panel = {0, 0, (float)w->width, (float)w->height};
  GuiPanel(panel, "Editor");

  Rectangle src = {0, 0, (float)ctx->renderer.viewport_editor.texture.width,
                   -(float)ctx->renderer.viewport_editor.texture.height};

  Rectangle dst = {panel.x + 1, panel.y + 24, panel.width - 2,
                   panel.height - 25};

  DrawTexturePro(ctx->renderer.viewport_editor.texture, src, dst, Vector2Zero(),
                 0.0f, WHITE);
}

void editor_viewport_render(CoreContext *ctx) {
  BeginTextureMode(ctx->renderer.viewport_editor);
  ClearBackground(DARKGRAY);

  BeginMode3D(ctx->renderer.camera);
  DrawGrid(20, 1.0f);
  EndMode3D();

  EndTextureMode();
}
