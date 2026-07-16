#include "core.h"
#include "config.h"
#include "gui/editor_viewport.h"
#include "raylib.h"
#include "renderer.h"
#include <assert.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "dark/style_dark.h"

static CoreContext **get_core_context_ptr_internal(void) {
  static CoreContext *g_ctx = NULL;
  return &g_ctx;
}

CoreContext *get_core_context(void) { return *get_core_context_ptr_internal(); }

void set_core_context(CoreContext *ctx) {
  *get_core_context_ptr_internal() = ctx;
}

void core_context_init(void) {
  CoreContext *ctx = get_core_context();
  (void)ctx;
}

static void core_init_window(CoreContext *ctx) {
  SetTraceLogLevel(LOG_WARNING);

  window_property_init();
  WindowProperty *window = get_window_property();
  SetConfigFlags(window->flags);
  InitWindow(window->width, window->height, window->title);

  renderer_context_init(ctx);
  GuiLoadStyleDark();
}

static void core_close_window(CoreContext *ctx) {
  (void)ctx;
  CloseWindow();
}

static void core_load_content(CoreContext *ctx) { (void)ctx; }

static void core_loop_update(CoreContext *ctx) {
  (void)ctx;
  if (IsWindowResized()) {
    WindowProperty *w = get_window_property();
    w->width = GetScreenWidth();
    w->height = GetScreenHeight();
  }
}

static void core_loop_render(CoreContext *ctx) { editor_viewport_panel(ctx); }

static void core_loop(CoreContext *ctx) {
  while (!WindowShouldClose()) {
    core_loop_update(ctx);
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    core_loop_render(ctx);
    EndDrawing();
  }
}

void core_run(void) {
  static CoreContext ctx = {0};
  set_core_context(&ctx);
  CoreContext *ctx_ptr = get_core_context();

  core_init_window(ctx_ptr);
  core_load_content(ctx_ptr);
  core_loop(ctx_ptr);
  core_close_window(ctx_ptr);
}
