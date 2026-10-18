#include "dev/rhi/opengl/opengl.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_video.h"
#include "dev/sdl_context.h"
#include "glad/gl.h"
#include "sustain.h"

typedef struct GL_State {
	SDL_GLContext context;
} GL_State;

static GL_State *gl_state(void)
{
	static GL_State state;
	return &state;
}

static GLADapiproc gl_get_proc(const char *name)
{
	return SDL_GL_GetProcAddress(name);
}

static int gl_init(const char *title, int w, int h)
{
	GL_State *state = gl_state();
	int pw = 0;
	int ph = 0;

	/* SDL_GL_SetAttribute fails silently before video init */
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize video: %s", SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if (sdl_context_init(title, w, h, SDL_WINDOW_OPENGL) != 0) {
		return -1;
	}

	state->context = SDL_GL_CreateContext(get_sdl_context()->window);
	if (state->context == NULL) {
		SDL_Log("Couldn't create GL context: %s", SDL_GetError());
		return -1;
	}

	if (gladLoadGL(gl_get_proc) == 0) {
		SDL_Log("Couldn't load GL 4.6 functions");
		return -1;
	}

	SDL_GL_SetSwapInterval(1);

	if (SDL_GetWindowSizeInPixels(get_sdl_context()->window, &pw, &ph)) {
		glViewport(0, 0, pw, ph);
	}

	glEnable(GL_DEPTH_TEST);
	SDL_Log("GL: %s", (const char *)glGetString(GL_VERSION));
	return 0;
}

static void gl_clear(Color color)
{
	glClearColor((float)color.r / 255.0f, (float)color.g / 255.0f,
		     (float)color.b / 255.0f, (float)color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void gl_present(void) { SDL_GL_SwapWindow(get_sdl_context()->window); }

const RHI_Ops gl_ops = {
    .init = gl_init,
    .clear = gl_clear,
    .present = gl_present,
};
