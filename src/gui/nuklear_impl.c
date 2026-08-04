// NOLINTBEGIN(llvm-include-order)
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_IMPLEMENTATION
#include "nuklear.h"

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
// NOLINTEND(llvm-include-order)

#include "gui/nuklear_impl.h"
#include "renderer/glutil.h"
#include "renderer/shader.h"
#include "shaders_generated.h"
#include <SDL3/SDL.h>
#include <assert.h>
#include <glad/gl.h>
#include <stdlib.h>

struct nk_gl_vertex {
	float position[2];
	float uv[2];
	nk_byte col[4];
};

struct nk_gl_device {
	struct nk_buffer cmds;
	struct nk_draw_null_texture tex_null;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint prog;
	GLint uniform_tex;
	GLint uniform_proj;
	GLuint font_tex;
};

static struct nk_font_atlas *get_atlas_internal(void)
{
	static struct nk_font_atlas atlas;
	return &atlas;
}

static struct nk_gl_device *get_device_internal(void)
{
	static struct nk_gl_device device;
	return &device;
}

static int nk_gl_device_create(struct nk_gl_device *dev)
{
	GLShader shader = gl_shader_create(shader_ui_vert, shader_ui_frag);
	if (shader.id == 0) {
		return 0;
	}

	dev->prog = shader.id;
	dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
	dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");

	GLsizei vsize = (GLsizei)sizeof(struct nk_gl_vertex);
	size_t vp = NK_OFFSETOF(struct nk_gl_vertex, position);
	size_t vt = NK_OFFSETOF(struct nk_gl_vertex, uv);
	size_t vc = NK_OFFSETOF(struct nk_gl_vertex, col);

	glGenBuffers(1, &dev->vbo);
	glGenBuffers(1, &dev->ebo);
	glGenVertexArrays(1, &dev->vao);

	glBindVertexArray(dev->vao);
	glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vsize,
			      gl_attrib_offset(vp));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vsize,
			      gl_attrib_offset(vt));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, vsize,
			      gl_attrib_offset(vc));

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	nk_buffer_init_default(&dev->cmds);
	return 1;
}

static void nk_gl_device_upload_atlas(struct nk_gl_device *dev,
				      const void *image, int width, int height)
{
	glGenTextures(1, &dev->font_tex);
	glBindTexture(GL_TEXTURE_2D, dev->font_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

static void nk_gl_device_destroy(struct nk_gl_device *dev)
{
	glDeleteProgram(dev->prog);
	glDeleteTextures(1, &dev->font_tex);
	glDeleteBuffers(1, &dev->vbo);
	glDeleteBuffers(1, &dev->ebo);
	glDeleteVertexArrays(1, &dev->vao);
	nk_buffer_free(&dev->cmds);
}

struct nk_context *nuklear_init(int font_size)
{
	struct nk_context *ctx =
	    (struct nk_context *)calloc(1, sizeof(struct nk_context));
	if (ctx == NULL) {
		return NULL;
	}

	if (!nk_gl_device_create(get_device_internal())) {
		free(ctx);
		return NULL;
	}

	nk_init_default(ctx, 0);

	struct nk_font_atlas *atlas = get_atlas_internal();
	struct nk_gl_device *dev = get_device_internal();

	nk_font_atlas_init_default(atlas);
	nk_font_atlas_begin(atlas);
	struct nk_font *font =
	    nk_font_atlas_add_default(atlas, (float)font_size, NULL);

	const void *image = NULL;
	int w = 0;
	int h = 0;
	image = nk_font_atlas_bake(atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	nk_gl_device_upload_atlas(dev, image, w, h);
	nk_font_atlas_end(atlas, nk_handle_id((int)dev->font_tex),
			  &dev->tex_null);

	if (font) {
		nk_style_set_font(ctx, &font->handle);
	}
	return ctx;
}

void nuklear_shutdown(struct nk_context *ctx)
{
	nk_font_atlas_clear(get_atlas_internal());
	nk_free(ctx);
	free(ctx);
	nk_gl_device_destroy(get_device_internal());
}

static void nuklear_handle_key(struct nk_context *ctx, SDL_Keycode sym,
			       bool down, bool ctrl_down)
{
	switch (sym) {
	case SDLK_RSHIFT:
	case SDLK_LSHIFT:
		nk_input_key(ctx, NK_KEY_SHIFT, down);
		break;
	case SDLK_DELETE:
		nk_input_key(ctx, NK_KEY_DEL, down);
		break;
	case SDLK_KP_ENTER:
	case SDLK_RETURN:
		nk_input_key(ctx, NK_KEY_ENTER, down);
		break;
	case SDLK_TAB:
		nk_input_key(ctx, NK_KEY_TAB, down);
		break;
	case SDLK_BACKSPACE:
		nk_input_key(ctx, NK_KEY_BACKSPACE, down);
		break;
	case SDLK_HOME:
		nk_input_key(ctx, NK_KEY_TEXT_START, down);
		nk_input_key(ctx, NK_KEY_SCROLL_START, down);
		break;
	case SDLK_END:
		nk_input_key(ctx, NK_KEY_TEXT_END, down);
		nk_input_key(ctx, NK_KEY_SCROLL_END, down);
		break;
	case SDLK_PAGEDOWN:
		nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
		break;
	case SDLK_PAGEUP:
		nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
		break;
	case SDLK_Z:
		nk_input_key(ctx, NK_KEY_TEXT_UNDO,
			     (nk_bool)(down && ctrl_down));
		break;
	case SDLK_R:
		nk_input_key(ctx, NK_KEY_TEXT_REDO,
			     (nk_bool)(down && ctrl_down));
		break;
	case SDLK_C:
		nk_input_key(ctx, NK_KEY_COPY, (nk_bool)(down && ctrl_down));
		break;
	case SDLK_V:
		nk_input_key(ctx, NK_KEY_PASTE, (nk_bool)(down && ctrl_down));
		break;
	case SDLK_X:
		nk_input_key(ctx, NK_KEY_CUT, (nk_bool)(down && ctrl_down));
		break;
	case SDLK_B:
		nk_input_key(ctx, NK_KEY_TEXT_LINE_START,
			     (nk_bool)(down && ctrl_down));
		break;
	case SDLK_E:
		nk_input_key(ctx, NK_KEY_TEXT_LINE_END,
			     (nk_bool)(down && ctrl_down));
		break;
	case SDLK_UP:
		nk_input_key(ctx, NK_KEY_UP, down);
		break;
	case SDLK_DOWN:
		nk_input_key(ctx, NK_KEY_DOWN, down);
		break;
	case SDLK_ESCAPE:
		nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, down);
		break;
	case SDLK_A:
		if (ctrl_down) {
			nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, down);
		}
		break;
	case SDLK_LEFT:
		if (ctrl_down) {
			nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
		} else {
			nk_input_key(ctx, NK_KEY_LEFT, down);
		}
		break;
	case SDLK_RIGHT:
		if (ctrl_down) {
			nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
		} else {
			nk_input_key(ctx, NK_KEY_RIGHT, down);
		}
		break;
	default:
		break;
	}
}

void nuklear_handle_event(struct nk_context *ctx, const SDL_Event *evt)
{
	bool ctrl_down = (SDL_GetModState() & SDL_KMOD_CTRL) != 0;

	switch (evt->type) {
	case SDL_EVENT_KEY_UP:
	case SDL_EVENT_KEY_DOWN:
		nuklear_handle_key(ctx, evt->key.key,
				   evt->type == SDL_EVENT_KEY_DOWN, ctrl_down);
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		bool down = evt->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
		int x = (int)evt->button.x;
		int y = (int)evt->button.y;
		switch (evt->button.button) {
		case SDL_BUTTON_LEFT:
			if (evt->button.clicks > 1) {
				nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y,
						down);
			}
			nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
			break;
		case SDL_BUTTON_MIDDLE:
			nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
			break;
		case SDL_BUTTON_RIGHT:
			nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
			break;
		case SDL_BUTTON_X1:
			nk_input_button(ctx, NK_BUTTON_X1, x, y, down);
			break;
		case SDL_BUTTON_X2:
			nk_input_button(ctx, NK_BUTTON_X2, x, y, down);
			break;
		default:
			break;
		}
		break;
	}
	case SDL_EVENT_MOUSE_MOTION:
		nk_input_motion(ctx, (int)evt->motion.x, (int)evt->motion.y);
		break;
	case SDL_EVENT_TEXT_INPUT: {
		nk_glyph glyph = {0};
		SDL_strlcpy(glyph, evt->text.text, sizeof(glyph));
		nk_input_glyph(ctx, glyph);
		break;
	}
	case SDL_EVENT_MOUSE_WHEEL:
		nk_input_scroll(ctx, nk_vec2(evt->wheel.x, evt->wheel.y));
		break;
	default:
		break;
	}
}

void nuklear_render(struct nk_context *ctx, int width, int height)
{
	struct nk_gl_device *dev = get_device_internal();

	static const struct nk_draw_vertex_layout_element vertex_layout[] = {
	    {NK_VERTEX_POSITION, NK_FORMAT_FLOAT,
	     NK_OFFSETOF(struct nk_gl_vertex, position)},
	    {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT,
	     NK_OFFSETOF(struct nk_gl_vertex, uv)},
	    {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8,
	     NK_OFFSETOF(struct nk_gl_vertex, col)},
	    {NK_VERTEX_LAYOUT_END}};

	struct nk_convert_config config = {0};
	config.vertex_layout = vertex_layout;
	config.vertex_size = sizeof(struct nk_gl_vertex);
	config.vertex_alignment = NK_ALIGNOF(struct nk_gl_vertex);
	config.tex_null = dev->tex_null;
	config.circle_segment_count = 22;
	config.curve_segment_count = 22;
	config.arc_segment_count = 22;
	config.global_alpha = 1.0f;
	config.shape_AA = NK_ANTI_ALIASING_ON;
	config.line_AA = NK_ANTI_ALIASING_ON;

	struct nk_buffer vbuf;
	struct nk_buffer ebuf;
	nk_buffer_init_default(&vbuf);
	nk_buffer_init_default(&ebuf);

	nk_flags result = nk_convert(ctx, &dev->cmds, &vbuf, &ebuf, &config);
	assert(result == NK_CONVERT_SUCCESS);

	GLfloat ortho[4][4] = {
	    {2.0f, 0.0f, 0.0f, 0.0f},
	    {0.0f, -2.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, -1.0f, 0.0f},
	    {-1.0f, 1.0f, 0.0f, 1.0f},
	};
	ortho[0][0] /= (GLfloat)width;
	ortho[1][1] /= (GLfloat)height;

	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	glUseProgram(dev->prog);
	glUniform1i(dev->uniform_tex, 0);
	glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);

	glBindVertexArray(dev->vao);
	glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

	const void *vertices = nk_buffer_memory_const(&vbuf);
	const void *elements = nk_buffer_memory_const(&ebuf);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)nk_buffer_total(&vbuf),
		     vertices, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		     (GLsizeiptr)nk_buffer_total(&ebuf), elements,
		     GL_STREAM_DRAW);

	const struct nk_draw_command *cmd = NULL;
	const nk_draw_index *offset = NULL;
	nk_draw_foreach(cmd, ctx, &dev->cmds)
	{
		if (!cmd->elem_count) {
			continue;
		}
		glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
		glScissor((GLint)cmd->clip_rect.x,
			  (GLint)((float)height -
				  (cmd->clip_rect.y + cmd->clip_rect.h)),
			  (GLsizei)cmd->clip_rect.w, (GLsizei)cmd->clip_rect.h);
		glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count,
			       GL_UNSIGNED_SHORT, offset);
		offset += cmd->elem_count;
	}

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);

	nk_buffer_free(&vbuf);
	nk_buffer_free(&ebuf);
	nk_clear(ctx);
	nk_buffer_clear(&dev->cmds);
}
