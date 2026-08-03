#include "core/core.h"
#include "core/config.h"
#include "core/log.h"
#include "ecs/components.h"
#include "gui/gui.h"
#include "raylib.h"
#include "renderer/renderer.h"
#include <assert.h>

static CoreContext **get_core_context_ptr_internal(void)
{
	static CoreContext *g_ctx = NULL;
	return &g_ctx;
}

CoreContext *get_core_context(void) { return *get_core_context_ptr_internal(); }

void set_core_context(CoreContext *ctx)
{
	*get_core_context_ptr_internal() = ctx;
}

void core_context_init(void)
{
	CoreContext *ctx = get_core_context();

	int font_size = 12;
	struct nk_context *nk_ctx = InitNuklear(font_size);
	ctx->nk_ctx = nk_ctx;

	ctx->world = ecs_init();
	components_register(ctx->world);

	ctx->selected_entity = 0;
	ctx->hierarchy_query = ecs_query(
	    ctx->world, {.terms = {{ecs_id(GameObject)},
				   {.id = ecs_pair(EcsChildOf, EcsWildcard),
				    .oper = EcsNot}}});

	renderer_context_init(ctx);
	editor_camera_init(&ctx->editor_camera);
}

static void core_init_window(void)
{
	SetTraceLogLevel(LOG_WARNING);

	window_property_init();
	WindowProperty *window = get_window_property();
	SetConfigFlags(window->flags);
	InitWindow(window->width, window->height, window->title);
  log_printf(LOG_LEVEL_INFO, "window initialized");
}

static void core_close_window(CoreContext *ctx)
{
	if (ctx->hierarchy_query) {
		ecs_query_fini(ctx->hierarchy_query);
	}
	renderer_context_cleanup(&ctx->renderer);
	ecs_fini(ctx->world);
	UnloadNuklear(ctx->nk_ctx);
	CloseWindow();
  log_printf(LOG_LEVEL_INFO, "window closed");
}

static void core_load_content(CoreContext *ctx)
{
	// blue cube
	ecs_entity_t cube = ecs_new(ctx->world);
	ecs_set_name(ctx->world, cube, "Blue Cube");
	ecs_add(ctx->world, cube, GameObject);
	ecs_set(ctx->world, cube, Transform3D,
		{.position = {0.0f, 1.0f, 0.0f},
		 .rotation = {0.0f, 0.0f, 0.0f},
		 .scale = {2.0f, 2.0f, 2.0f}});
	ecs_set(ctx->world, cube, MeshRenderer,
		{.type = PRIMITIVE_CUBE, .color = (Color){80, 140, 220, 255}});

	// red sphere
	ecs_entity_t sphere = ecs_new(ctx->world);
	ecs_set_name(ctx->world, sphere, "Red Sphere");
	ecs_add(ctx->world, sphere, GameObject);
	ecs_set(ctx->world, sphere, Transform3D,
		{.position = {4.0f, 1.0f, 0.0f},
		 .rotation = {0.0f, 0.0f, 0.0f},
		 .scale = {1.0f, 1.0f, 1.0f}});
	ecs_set(ctx->world, sphere, MeshRenderer,
		{.type = PRIMITIVE_SPHERE, .color = (Color){220, 80, 80, 255}});

	// child cube
	ecs_entity_t child_cube = ecs_new(ctx->world);
	ecs_set_name(ctx->world, child_cube, "Child Cube");
	ecs_add(ctx->world, child_cube, GameObject);
	ecs_add_pair(ctx->world, child_cube, EcsChildOf, cube);
	ecs_set(ctx->world, child_cube, Transform3D,
		{.position = {0.0f, 2.0f, 0.0f},
		 .rotation = {0.0f, 0.0f, 0.0f},
		 .scale = {0.5f, 0.5f, 0.5f}});
	ecs_set(ctx->world, child_cube, MeshRenderer,
		{.type = PRIMITIVE_CUBE, .color = (Color){80, 220, 140, 255}});
}

static void core_loop_update(CoreContext *ctx)
{
	if (IsWindowResized()) {
		WindowProperty *w = get_window_property();
		w->width = GetScreenWidth();
		w->height = GetScreenHeight();
	}

	ecs_progress(ctx->world, GetFrameTime());
}

static void core_loop_render_ui(CoreContext *ctx) { gui_render_ui(ctx); }

static void core_loop_render(CoreContext *ctx)
{
	DrawNuklear(ctx->nk_ctx);

	if (ctx->renderer.viewport_initialized) {
		Texture2D vp_tex = ctx->renderer.viewport_rt.texture;
		Rectangle src = {0, 0, (float)vp_tex.width,
				 -(float)vp_tex.height};
		Rectangle dst = ctx->renderer.viewport_draw_rect;
		DrawTexturePro(vp_tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
	}
}

static void core_loop(CoreContext *ctx)
{
	while (!WindowShouldClose()) {
		UpdateNuklear(ctx->nk_ctx);
		core_loop_update(ctx);
		core_loop_render_ui(ctx);
		BeginDrawing();
		ClearBackground(BLACK);
		core_loop_render(ctx);
		EndDrawing();
	}
}

void core_run(void)
{
	static CoreContext ctx = {0};
	set_core_context(&ctx);
	CoreContext *ctx_ptr = get_core_context();

	core_init_window();
	core_context_init();
	core_load_content(ctx_ptr);
	core_loop(ctx_ptr);
	core_close_window(ctx_ptr);
}
