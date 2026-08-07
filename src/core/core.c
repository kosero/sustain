#include "core/core.h"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_version.h"
#include "SDL3/SDL_video.h"
#include "core/config.h"
#include "core/input.h"
#include "core/log.h"
#include "ecs/components.h"
#include "gui/gui.h"
#include "gui/microui_impl.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CoreContext **get_core_context_ptr_internal(void)
{
	static CoreContext *g_ctx = NULL;
	return &g_ctx;
}

static SDL_Window **get_window_ptr_internal(void)
{
	static SDL_Window *w = NULL;
	return &w;
}

static SDL_GLContext *get_gl_context_ptr_internal(void)
{
	static SDL_GLContext c = NULL;
	return &c;
}

static float *get_delta_time_ptr_internal(void)
{
	static float dt = 0.016f;
	return &dt;
}

CoreContext *get_core_context(void) { return *get_core_context_ptr_internal(); }

void set_core_context(CoreContext *ctx)
{
	*get_core_context_ptr_internal() = ctx;
}

float core_get_delta_time(void) { return *get_delta_time_ptr_internal(); }

SDL_Window *core_get_window(void) { return *get_window_ptr_internal(); }

static void core_update_delta_time(void)
{
	static Uint64 last_tick = 0;
	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 freq = SDL_GetPerformanceFrequency();
	float dt = (last_tick == 0)
		       ? 0.016f
		       : (float)((double)(now - last_tick) / (double)freq);
	last_tick = now;
	if (dt > 0.1f) {
		dt = 0.1f;
	}
	*get_delta_time_ptr_internal() = dt;
}

int core_context_init(void)
{
	CoreContext *ctx = get_core_context();

	int font_size = 18;
	ctx->mu_ctx = microui_init(font_size);
	if (ctx->mu_ctx == NULL) {
		log_printf(LOG_LEVEL_ERROR, "microui init failed");
		return -1;
	}

	ctx->world = ecs_init();
	if (ctx->world == NULL) {
		log_printf(LOG_LEVEL_ERROR, "ecs init failed");
		return -1;
	}
	log_printf(LOG_LEVEL_INFO, "ecs world created");
	components_register(ctx->world);

	ctx->selected_entity = 0;
	ctx->hierarchy_query = ecs_query(
	    ctx->world, {.terms = {{ecs_id(GameObject)},
				   {.id = ecs_pair(EcsChildOf, EcsWildcard),
				    .oper = EcsNot}}});

	renderer_context_init(ctx);
	editor_camera_init(&ctx->editor_camera);
	log_printf(LOG_LEVEL_INFO, "core context ready");
	return 0;
}

static void core_sdl_cleanup(void)
{
	SDL_GLContext *gl = get_gl_context_ptr_internal();
	SDL_Window **win = get_window_ptr_internal();
	if (*gl != NULL) {
		SDL_GL_DestroyContext(*gl);
		*gl = NULL;
	}
	if (*win != NULL) {
		SDL_DestroyWindow(*win);
		*win = NULL;
	}
	SDL_Quit();
}

static int core_init_fatal(const char *message)
{
	log_printf(LOG_LEVEL_ERROR, "%s: %s", message, SDL_GetError());
	core_sdl_cleanup();
	return -1;
}

static int core_init_window(void)
{
	window_property_init();
	WindowProperty *window = get_window_property();

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		return core_init_fatal("SDL_Init failed");
	}

	int sdl_version = SDL_GetVersion();
	log_printf(LOG_LEVEL_INFO, "SDL %d.%d.%d initialized",
		   SDL_VERSIONNUM_MAJOR(sdl_version),
		   SDL_VERSIONNUM_MINOR(sdl_version),
		   SDL_VERSIONNUM_MICRO(sdl_version));

	const char *video_driver = SDL_GetCurrentVideoDriver();
	log_printf(LOG_LEVEL_INFO, "video driver: %s",
		   video_driver != NULL ? video_driver : "unknown");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window **window_ptr = get_window_ptr_internal();
	SDL_GLContext *gl_context_ptr = get_gl_context_ptr_internal();

	*window_ptr = SDL_CreateWindow(
	    window->title, window->width, window->height,
	    SDL_WINDOW_OPENGL | (SDL_WindowFlags)window->flags);
	if (*window_ptr == NULL) {
		return core_init_fatal("SDL_CreateWindow failed");
	}
	log_printf(LOG_LEVEL_INFO, "window created: %dx%d '%s'", window->width,
		   window->height, window->title);

	const int gl_versions[][2] = {{4, 6}, {4, 1}};
	for (size_t i = 0; i < sizeof(gl_versions) / sizeof(gl_versions[0]);
	     i++) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
				    gl_versions[i][0]);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
				    gl_versions[i][1]);
		*gl_context_ptr = SDL_GL_CreateContext(*window_ptr);
		if (*gl_context_ptr != NULL) {
			break;
		}
		if (i + 1 < sizeof(gl_versions) / sizeof(gl_versions[0])) {
			log_printf(LOG_LEVEL_WARN,
				   "GL %d.%d context failed, trying %d.%d: %s",
				   gl_versions[i][0], gl_versions[i][1],
				   gl_versions[i + 1][0], gl_versions[i + 1][1],
				   SDL_GetError());
		}
	}
	if (*gl_context_ptr == NULL) {
		return core_init_fatal("SDL_GL_CreateContext failed");
	}

	if (!SDL_GL_SetSwapInterval(1)) {
		log_printf(LOG_LEVEL_WARN, "vsync unavailable: %s",
			   SDL_GetError());
	} else {
		log_printf(LOG_LEVEL_INFO, "vsync enabled");
	}

	if (gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0) {
		log_printf(LOG_LEVEL_ERROR, "gladLoadGL failed");
		core_sdl_cleanup();
		return -1;
	}

	log_printf(LOG_LEVEL_INFO, "opengl: %s | %s", glGetString(GL_RENDERER),
		   glGetString(GL_VERSION));
	log_printf(LOG_LEVEL_INFO, "glsl: %s",
		   glGetString(GL_SHADING_LANGUAGE_VERSION));
	return 0;
}

static void core_close_window(CoreContext *ctx)
{
	log_printf(LOG_LEVEL_INFO,
		   "shutdown: %llu frames in %.2fs (avg %.1f fps)",
		   (unsigned long long)ctx->frame_count, ctx->run_time_seconds,
		   ctx->run_time_seconds > 0.0
		       ? (double)ctx->frame_count / ctx->run_time_seconds
		       : 0.0);

	if (ctx->hierarchy_query) {
		ecs_query_fini(ctx->hierarchy_query);
	}
	log_printf(LOG_LEVEL_DEBUG, "renderer cleanup");
	renderer_context_cleanup(&ctx->renderer);
	log_printf(LOG_LEVEL_DEBUG, "ecs cleanup");
	ecs_fini(ctx->world);
	log_printf(LOG_LEVEL_DEBUG, "microui cleanup");
	microui_shutdown(ctx->mu_ctx);

	core_sdl_cleanup();
	log_printf(LOG_LEVEL_INFO, "shutdown complete");
}

static ecs_entity_t create_primitive(CoreContext *ctx, const char *name,
				     PrimitiveType type, Color color,
				     vec3s position, vec3s scale,
				     ecs_entity_t parent)
{
	ecs_entity_t e = ecs_new(ctx->world);
	ecs_set_name(ctx->world, e, name);
	ecs_add(ctx->world, e, GameObject);
	if (parent != 0) {
		ecs_add_pair(ctx->world, e, EcsChildOf, parent);
	}
	ecs_set(ctx->world, e, Transform3D,
		{.position = position,
		 .rotation = {{0.0f, 0.0f, 0.0f}},
		 .scale = scale});
	ecs_set(ctx->world, e, MeshRenderer, {.type = type, .color = color});
	log_printf(LOG_LEVEL_INFO, "created '%s' (%s)", name,
		   type == PRIMITIVE_CUBE ? "cube" : "sphere");
	return e;
}

static void core_load_content(CoreContext *ctx)
{
	ecs_entity_t cube = create_primitive(
	    ctx, "Blue Cube", PRIMITIVE_CUBE, (Color){80, 140, 220, 255},
	    (vec3s){{0.0f, 1.0f, 0.0f}}, (vec3s){{2.0f, 2.0f, 2.0f}}, 0);

	create_primitive(ctx, "Red Sphere", PRIMITIVE_SPHERE,
			 (Color){220, 80, 80, 255}, (vec3s){{4.0f, 1.0f, 0.0f}},
			 (vec3s){{1.0f, 1.0f, 1.0f}}, 0);

	create_primitive(
	    ctx, "Child Cube", PRIMITIVE_CUBE, (Color){80, 220, 140, 255},
	    (vec3s){{0.0f, 2.0f, 0.0f}}, (vec3s){{0.5f, 0.5f, 0.5f}}, cube);

	log_printf(LOG_LEVEL_INFO, "scene loaded: %d game objects",
		   ecs_count(ctx->world, GameObject));
}

static void core_loop_update(CoreContext *ctx)
{
	if (input_window_resized()) {
		WindowProperty *w = get_window_property();
		w->width = input_window_width();
		w->height = input_window_height();
		log_printf(LOG_LEVEL_DEBUG, "window resized to %dx%d", w->width,
			   w->height);
	}

	ecs_progress(ctx->world, *get_delta_time_ptr_internal());
}

static void core_loop_render_ui(CoreContext *ctx) { gui_render_ui(ctx); }

static void core_loop_render(CoreContext *ctx)
{
	WindowProperty *w = get_window_property();

	glViewport(0, 0, w->width, w->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	microui_render(ctx->mu_ctx, w->width, w->height);

	if (ctx->renderer.viewport_initialized) {
		renderer_blit_viewport(&ctx->renderer,
				       ctx->renderer.viewport_draw_rect,
				       w->width, w->height);
	}
}

static void core_loop(CoreContext *ctx)
{
	Uint64 start_ticks = SDL_GetPerformanceCounter();
	Uint64 frame_count = 0;

	while (!input_quit_requested()) {
		core_update_delta_time();
		input_frame_begin();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			microui_handle_event(ctx->mu_ctx, &evt);
			input_handle_event(&evt);
		}

		core_loop_update(ctx);
		core_loop_render_ui(ctx);
		microui_update_text_input(ctx->mu_ctx);
		core_loop_render(ctx);

		SDL_GL_SwapWindow(*get_window_ptr_internal());
		frame_count++;
	}

	Uint64 end_ticks = SDL_GetPerformanceCounter();
	ctx->frame_count = frame_count;
	ctx->run_time_seconds = (double)(end_ticks - start_ticks) /
				(double)SDL_GetPerformanceFrequency();
}

static void core_restrict_egl_vendors(void)
{
#ifdef __linux__
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	const char *existing = getenv("__EGL_VENDOR_LIBRARY_FILENAMES");
	if (existing != NULL && existing[0] != '\0') {
		return;
	}
	const char *vendor_dir = "/usr/share/glvnd/egl_vendor.d";
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	DIR *dir = opendir(vendor_dir);
	if (dir == NULL) {
		return;
	}

	enum { VENDOR_MAX = 16 };
	enum { VENDOR_PATH_MAX = 256 };
	enum { JOINED_MAX = 4096 };
	char paths[VENDOR_MAX][VENDOR_PATH_MAX];
	int count = 0;

	struct dirent *entry = NULL;
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	while ((entry = readdir(dir)) != NULL && count < VENDOR_MAX) {
		const char *name = entry->d_name;
		size_t name_len = strlen(name);
		if (name_len < 6 || strcmp(&name[name_len - 5], ".json") != 0) {
			continue;
		}
		if (strstr(name, "nvidia") != NULL) {
			continue;
		}
		(void)snprintf(paths[count], sizeof(paths[count]), "%s/%s",
			       vendor_dir, name);
		count++;
	}
	closedir(dir);

	if (count == 0) {
		return;
	}

	char joined[JOINED_MAX];
	size_t used = 0;
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			joined[used++] = ':';
		}
		size_t path_len = strlen(paths[i]);
		size_t room = sizeof(joined) - used - 1;
		if (path_len > room) {
			path_len = room;
		}
		memcpy(&joined[used], paths[i], path_len);
		used += path_len;
	}
	joined[used] = '\0';

	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	(void)setenv("__EGL_VENDOR_LIBRARY_FILENAMES", joined, 1);
#endif
}

int core_run(void)
{
	static CoreContext ctx = {0};
	set_core_context(&ctx);
	CoreContext *ctx_ptr = get_core_context();

	core_restrict_egl_vendors();

	if (core_init_window() != 0) {
		return 1;
	}
	if (core_context_init() != 0) {
		core_sdl_cleanup();
		return 1;
	}
	core_load_content(ctx_ptr);
	core_loop(ctx_ptr);
	core_close_window(ctx_ptr);
	return 0;
}
