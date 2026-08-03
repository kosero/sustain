#ifndef SUSTAIN_CORE_H
#define SUSTAIN_CORE_H

#include "renderer/editor_camera.h"
#include "raylib-nuklear.h"
#include "renderer/renderer.h"
#include <flecs.h>

typedef struct CoreContext {
	struct nk_context *nk_ctx;
	ecs_world_t *world;
	ecs_query_t *hierarchy_query;
	ecs_entity_t selected_entity;
	RendererContext renderer;
	EditorCamera editor_camera;
} CoreContext;

CoreContext *get_core_context(void);
void set_core_context(CoreContext *ctx);
void core_context_init(void);

void core_run(void);

#endif // SUSTAIN_CORE_H
