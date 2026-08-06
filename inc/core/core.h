#ifndef SUSTAIN_CORE_H
#define SUSTAIN_CORE_H

#include "renderer/editor_camera.h"
#include "renderer/renderer.h"
#include <flecs.h>
#include <stdint.h>
struct nk_context;
struct SDL_Window;

typedef struct CoreContext {
	struct nk_context *nk_ctx;
	ecs_world_t *world;
	ecs_query_t *hierarchy_query;
	ecs_entity_t selected_entity;
	RendererContext renderer;
	EditorCamera editor_camera;
	double run_time_seconds;
	uint64_t frame_count;
} CoreContext;

CoreContext *get_core_context(void);
void set_core_context(CoreContext *ctx);
int core_context_init(void);

float core_get_delta_time(void);
struct SDL_Window *core_get_window(void);

int core_run(void);

#endif // SUSTAIN_CORE_H
