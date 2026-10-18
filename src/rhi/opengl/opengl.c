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
	int w;
	int h;
	GLuint program;
	GLuint vao;
	GLuint vbo;
	GLint offset_loc;
	GLint size_loc;
	GLint color_loc;
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

static GLuint gl_compile_shader(GLenum type, const char *src)
{
	GLuint shader = glCreateShader(type);
	char log[512];
	GLint ok = 0;

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		glGetShaderInfoLog(shader, (GLsizei)sizeof(log), NULL, log);
		SDL_Log("Couldn't compile shader: %s", log);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

static int gl_pipeline_init(void)
{
	GL_State *state = gl_state();
	float quad[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, };
	GLuint vs = gl_compile_shader(GL_VERTEX_SHADER, pixel_vert_src);
	GLuint fs = gl_compile_shader(GL_FRAGMENT_SHADER, pixel_frag_src);
	GLint ok = 0;

	if (vs == 0 || fs == 0) {
		return -1;
	}

	state->program = glCreateProgram();
	glAttachShader(state->program, vs);
	glAttachShader(state->program, fs);
	glLinkProgram(state->program);
	glGetProgramiv(state->program, GL_LINK_STATUS, &ok);
	glDeleteShader(vs);
	glDeleteShader(fs);
	if (!ok) {
		char log[512];

		glGetProgramInfoLog(state->program, (GLsizei)sizeof(log), NULL,
				    log);
		SDL_Log("Couldn't link shader program: %s", log);
		return -1;
	}

	state->offset_loc = glGetUniformLocation(state->program, "u_offset");
	state->size_loc = glGetUniformLocation(state->program, "u_size");
	state->color_loc = glGetUniformLocation(state->program, "u_color");

	glGenVertexArrays(1, &state->vao);
	glGenBuffers(1, &state->vbo);
	glBindVertexArray(state->vao);
	glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(quad), quad,
		     GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			      (GLsizei)(2 * sizeof(float)), (const void *)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return 0;
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

	/* no depth test: 2D draws are painter-ordered */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	state->w = w;
	state->h = h;

	if (gl_pipeline_init() != 0) {
		return -1;
	}

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

static void gl_draw_rect(int x, int y, int w, int h, Color color)
{
	GL_State *state = gl_state();
	float ox = (((float)x / (float)state->w) * 2.0f) - 1.0f;
	float oy = 1.0f - (((float)(y + h) / (float)state->h) * 2.0f);

	glUseProgram(state->program);
	glUniform2f(state->offset_loc, ox, oy);
	glUniform2f(state->size_loc,
		     (((float)w / (float)state->w) * 2.0f),
		     (((float)h / (float)state->h) * 2.0f));
	glUniform4f(state->color_loc, (float)color.r / 255.0f,
		     (float)color.g / 255.0f, (float)color.b / 255.0f,
		     (float)color.a / 255.0f);
	glBindVertexArray(state->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

const RHI_Ops gl_ops = {
    .init = gl_init,
    .clear = gl_clear,
    .present = gl_present,
    .draw_rect = gl_draw_rect,
};
