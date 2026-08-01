#include "renderer.h"
#include "components.h"
#include "core.h"
#include "raylib.h"
#include "rlgl.h"
#include <assert.h>

void renderer_context_init(struct CoreContext *ctx)
{
	assert(ctx != NULL && "Core context must not be NULL");
	
	RendererContext *rctx = &ctx->renderer;
	int initial_w = 640;
	int initial_h = 480;
	
	rctx->viewport_rt = LoadRenderTexture(initial_w, initial_h);
	rctx->viewport_width = initial_w;
	rctx->viewport_height = initial_h;
	rctx->viewport_initialized = true;

	assert(rctx->viewport_width == 640 && "Viewport width initialized incorrectly");
	assert(rctx->viewport_height == 480 && "Viewport height initialized incorrectly");

	rctx->render_query = ecs_query(
	    ctx->world,
	    {.terms = {{ecs_id(Transform3D)}, {ecs_id(MeshRenderer)}}});
}

void renderer_viewport_resize(RendererContext *rctx, int width, int height)
{
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

void renderer_draw_viewport(RendererContext *rctx, Camera3D *camera,
			    ecs_world_t *world)
{
	BeginTextureMode(rctx->viewport_rt);
	ClearBackground((Color){30, 30, 35, 255});

	BeginMode3D(*camera);

	DrawGrid(20, 1.0f);

	ecs_iter_t it = ecs_query_iter(world, rctx->render_query);
	while (ecs_query_next(&it)) {
		Transform3D *t = ecs_field(&it, Transform3D, 0);
		MeshRenderer *m = ecs_field(&it, MeshRenderer, 1);

		for (int i = 0; i < it.count; i++) {
			rlPushMatrix();
			rlTranslatef(t[i].position.x, t[i].position.y,
				     t[i].position.z);

			// euler rotation
			rlRotatef(t[i].rotation.y, 0.0f, 1.0f, 0.0f);
			rlRotatef(t[i].rotation.x, 1.0f, 0.0f, 0.0f);
			rlRotatef(t[i].rotation.z, 0.0f, 0.0f, 1.0f);

			if (m[i].type == PRIMITIVE_CUBE) {
				DrawCube((Vector3){0, 0, 0}, t[i].scale.x,
					 t[i].scale.y, t[i].scale.z,
					 m[i].color);
				DrawCubeWires((Vector3){0, 0, 0}, t[i].scale.x,
					      t[i].scale.y, t[i].scale.z,
					      BLACK);
			} else if (m[i].type == PRIMITIVE_SPHERE) {
				DrawSphere((Vector3){0, 0, 0}, t[i].scale.x,
					   m[i].color);
			}

			rlPopMatrix();
		}
	}

	EndMode3D();
	EndTextureMode();
}

void renderer_context_cleanup(RendererContext *rctx)
{
	if (rctx->render_query) {
		ecs_query_fini(rctx->render_query);
	}

	if (rctx->viewport_initialized) {
		UnloadRenderTexture(rctx->viewport_rt);
		rctx->viewport_initialized = false;
	}
}
