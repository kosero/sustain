#include "microui.h"

#include "atlas.inl"

#include "SDL3/SDL_keyboard.h"
#include "core/core.h"
#include "gui/microui_impl.h"
#include "renderer/glutil.h"
#include "renderer/shader.h"
#include "shaders_generated.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

enum { MU_MAX_QUADS = 16384 };

struct mu_gl_vertex {
	float position[2];
	float uv[2];
	unsigned char col[4];
};

struct mu_gl_device {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint prog;
	GLint uniform_tex;
	GLint uniform_proj;
	GLuint font_tex;
	int line_height;
	struct mu_gl_vertex verts[MU_MAX_QUADS * 4];
	int quad_count;
};

static struct mu_gl_device *get_device_internal(void)
{
	static struct mu_gl_device device;
	return &device;
}

static void mu_gl_device_create_texture(struct mu_gl_device *dev)
{
	glGenTextures(1, &dev->font_tex);
	glBindTexture(GL_TEXTURE_2D, dev->font_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_WIDTH, ATLAS_HEIGHT, 0,
		     GL_RED, GL_UNSIGNED_BYTE, atlas_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void mu_gl_device_fill_indices(void)
{
	GLuint indices[MU_MAX_QUADS * 6];
	for (int q = 0; q < MU_MAX_QUADS; q++) {
		unsigned int base = (unsigned int)((size_t)q * 4);
		GLuint *idx = &indices[(size_t)q * 6];
		idx[0] = base + 0;
		idx[1] = base + 1;
		idx[2] = base + 2;
		idx[3] = base + 2;
		idx[4] = base + 3;
		idx[5] = base + 1;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		     GL_STATIC_DRAW);
}

static int mu_gl_device_create(struct mu_gl_device *dev)
{
	GLShader shader = gl_shader_create(shader_ui_vert, shader_ui_frag);
	if (shader.id == 0) {
		return 0;
	}

	dev->prog = shader.id;
	dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
	dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");

	GLsizei vsize = (GLsizei)sizeof(struct mu_gl_vertex);
	glGenBuffers(1, &dev->vbo);
	glGenBuffers(1, &dev->ebo);
	glGenVertexArrays(1, &dev->vao);

	glBindVertexArray(dev->vao);
	glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
	    0, 2, GL_FLOAT, GL_FALSE, vsize,
	    gl_attrib_offset(offsetof(struct mu_gl_vertex, position)));
	glVertexAttribPointer(
	    1, 2, GL_FLOAT, GL_FALSE, vsize,
	    gl_attrib_offset(offsetof(struct mu_gl_vertex, uv)));
	glVertexAttribPointer(
	    2, 4, GL_UNSIGNED_BYTE, GL_TRUE, vsize,
	    gl_attrib_offset(offsetof(struct mu_gl_vertex, col)));

	mu_gl_device_fill_indices();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mu_gl_device_create_texture(dev);
	return 1;
}

static void mu_gl_device_destroy(struct mu_gl_device *dev)
{
	glDeleteProgram(dev->prog);
	glDeleteTextures(1, &dev->font_tex);
	glDeleteBuffers(1, &dev->vbo);
	glDeleteBuffers(1, &dev->ebo);
	glDeleteVertexArrays(1, &dev->vao);
}

static void buffer_flush(struct mu_gl_device *dev)
{
	if (dev->quad_count == 0) {
		return;
	}
	glBufferData(GL_ARRAY_BUFFER,
		     (GLsizeiptr)((size_t)dev->quad_count * 4 *
				  sizeof(struct mu_gl_vertex)),
		     dev->verts, GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, dev->quad_count * 6, GL_UNSIGNED_INT,
		       NULL);
	dev->quad_count = 0;
}

static void quad_push(struct mu_gl_device *dev, mu_Rect dst, mu_Rect src,
		      mu_Color color)
{
	if (dev->quad_count == MU_MAX_QUADS) {
		buffer_flush(dev);
	}

	int q = dev->quad_count++;
	struct mu_gl_vertex *v = &dev->verts[(size_t)q * 4];

	float u0 = (float)src.x / (float)ATLAS_WIDTH;
	float v0 = (float)src.y / (float)ATLAS_HEIGHT;
	float u1 = (float)(src.x + src.w) / (float)ATLAS_WIDTH;
	float v1 = (float)(src.y + src.h) / (float)ATLAS_HEIGHT;

	float x0 = (float)dst.x;
	float y0 = (float)dst.y;
	float x1 = (float)(dst.x + dst.w);
	float y1 = (float)(dst.y + dst.h);

	const float pos[4][2] = {{x0, y0}, {x1, y0}, {x0, y1}, {x1, y1}};
	const float uvs[4][2] = {{u0, v0}, {u1, v0}, {u0, v1}, {u1, v1}};
	for (int i = 0; i < 4; i++) {
		v[i].position[0] = pos[i][0];
		v[i].position[1] = pos[i][1];
		v[i].uv[0] = uvs[i][0];
		v[i].uv[1] = uvs[i][1];
		v[i].col[0] = color.r;
		v[i].col[1] = color.g;
		v[i].col[2] = color.b;
		v[i].col[3] = color.a;
	}
}

static void draw_rect(struct mu_gl_device *dev, mu_Rect rect, mu_Color color)
{
	quad_push(dev, rect, atlas[ATLAS_WHITE], color);
}

static void draw_text(struct mu_gl_device *dev, mu_Font font, const char *str,
		      mu_Vec2 pos, mu_Color color)
{
	(void)font;
	mu_Rect dst = {pos.x, pos.y, 0, 0};
	for (const char *p = str; *p; p++) {
		if ((*p & 0xc0) == 0x80) {
			continue;
		}
		int chr = mu_min((unsigned char)*p, 127);
		mu_Rect src = atlas[ATLAS_FONT + chr];
		dst.w = src.w;
		dst.h = src.h;
		quad_push(dev, dst, src, color);
		dst.x += dst.w;
	}
}

static void draw_icon(struct mu_gl_device *dev, int id, mu_Rect rect,
		      mu_Color color)
{
	mu_Rect src = atlas[id];
	int x = rect.x + ((rect.w - src.w) / 2);
	int y = rect.y + ((rect.h - src.h) / 2);
	quad_push(dev, mu_rect(x, y, src.w, src.h), src, color);
}

static int text_width(mu_Font font, const char *text, int len)
{
	(void)font;
	if (len < 0) {
		len = (int)strlen(text);
	}
	int res = 0;
	for (const char *p = text; *p && len--; p++) {
		if ((*p & 0xc0) == 0x80) {
			continue;
		}
		int chr = mu_min((unsigned char)*p, 127);
		res += atlas[ATLAS_FONT + chr].w;
	}
	return res;
}

static int text_height(mu_Font font)
{
	(void)font;
	return get_device_internal()->line_height;
}

struct mu_Context *microui_init(int font_size)
{
	struct mu_Context *ctx =
	    (struct mu_Context *)calloc(1, sizeof(struct mu_Context));
	if (ctx == NULL) {
		return NULL;
	}

	if (!mu_gl_device_create(get_device_internal())) {
		free(ctx);
		return NULL;
	}

	mu_init(ctx);
	ctx->text_width = text_width;
	ctx->text_height = text_height;

	get_device_internal()->line_height =
	    (font_size >= 8 && font_size <= 72) ? font_size : 18;

	mu_Style *s = ctx->style;
	s->colors[MU_COLOR_TEXT] = mu_color(210, 210, 210, 255);
	s->colors[MU_COLOR_BORDER] = mu_color(70, 70, 74, 255);
	s->colors[MU_COLOR_WINDOWBG] = mu_color(37, 37, 38, 255);
	s->colors[MU_COLOR_TITLEBG] = mu_color(30, 30, 30, 255);
	s->colors[MU_COLOR_TITLETEXT] = mu_color(220, 220, 220, 255);
	s->colors[MU_COLOR_PANELBG] = mu_color(45, 45, 48, 255);
	s->colors[MU_COLOR_BUTTON] = mu_color(55, 55, 60, 255);
	s->colors[MU_COLOR_BUTTONHOVER] = mu_color(70, 70, 76, 255);
	s->colors[MU_COLOR_BUTTONFOCUS] = mu_color(85, 85, 92, 255);
	s->colors[MU_COLOR_BASE] = mu_color(60, 60, 64, 255);
	s->colors[MU_COLOR_BASEHOVER] = mu_color(70, 70, 74, 255);
	s->colors[MU_COLOR_BASEFOCUS] = mu_color(80, 80, 86, 255);
	s->colors[MU_COLOR_SCROLLBASE] = mu_color(40, 40, 42, 255);
	s->colors[MU_COLOR_SCROLLTHUMB] = mu_color(90, 90, 96, 255);

	return ctx;
}

void microui_shutdown(struct mu_Context *ctx)
{
	mu_gl_device_destroy(get_device_internal());
	free(ctx);
}

static int button_map(int button)
{
	switch (button) {
	case SDL_BUTTON_LEFT:
		return MU_MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return MU_MOUSE_RIGHT;
	case SDL_BUTTON_MIDDLE:
		return MU_MOUSE_MIDDLE;
	default:
		return 0;
	}
}

static int key_map(SDL_Keycode key)
{
	switch (key) {
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		return MU_KEY_SHIFT;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		return MU_KEY_CTRL;
	case SDLK_LALT:
	case SDLK_RALT:
		return MU_KEY_ALT;
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		return MU_KEY_RETURN;
	case SDLK_BACKSPACE:
		return MU_KEY_BACKSPACE;
	default:
		return 0;
	}
}

void microui_handle_event(struct mu_Context *ctx, const SDL_Event *evt)
{
	switch (evt->type) {
	case SDL_EVENT_KEY_UP:
	case SDL_EVENT_KEY_DOWN: {
		int key = key_map(evt->key.key);
		if (key) {
			if (evt->type == SDL_EVENT_KEY_DOWN) {
				mu_input_keydown(ctx, key);
			} else {
				mu_input_keyup(ctx, key);
			}
		}
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP:
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		int button = button_map(evt->button.button);
		int x = (int)evt->button.x;
		int y = (int)evt->button.y;
		if (button) {
			if (evt->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				mu_input_mousedown(ctx, x, y, button);
			} else {
				mu_input_mouseup(ctx, x, y, button);
			}
		}
		break;
	}
	case SDL_EVENT_MOUSE_MOTION:
		mu_input_mousemove(ctx, (int)evt->motion.x, (int)evt->motion.y);
		break;
	case SDL_EVENT_TEXT_INPUT:
		mu_input_text(ctx, evt->text.text);
		break;
	case SDL_EVENT_MOUSE_WHEEL:
		mu_input_scroll(ctx, (int)evt->wheel.x,
				(int)(evt->wheel.y * -30.0f));
		break;
	default:
		break;
	}
}

static bool *get_text_input_active(void)
{
	static bool active = false;
	return &active;
}

void microui_update_text_input(struct mu_Context *ctx)
{
	bool want_active = (bool)(ctx->number_edit != 0);
	bool *active = get_text_input_active();
	if (want_active == *active) {
		return;
	}
	if (want_active) {
		SDL_StartTextInput(core_get_window());
	} else {
		SDL_StopTextInput(core_get_window());
	}
	*active = want_active;
}

void microui_render(struct mu_Context *ctx, int width, int height)
{
	struct mu_gl_device *dev = get_device_internal();

	GLfloat ortho[4][4] = {
	    {2.0f / (GLfloat)width, 0.0f, 0.0f, 0.0f},
	    {0.0f, -2.0f / (GLfloat)height, 0.0f, 0.0f},
	    {0.0f, 0.0f, -1.0f, 0.0f},
	    {-1.0f, 1.0f, 0.0f, 1.0f},
	};

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

	glBindTexture(GL_TEXTURE_2D, dev->font_tex);
	glScissor(0, 0, width, height);

	dev->quad_count = 0;

	mu_Command *cmd = NULL;
	while (mu_next_command(ctx, &cmd)) {
		switch (cmd->type) {
		case MU_COMMAND_TEXT:
			draw_text(dev, cmd->text.font, cmd->text.str,
				  cmd->text.pos, cmd->text.color);
			break;
		case MU_COMMAND_RECT:
			draw_rect(dev, cmd->rect.rect, cmd->rect.color);
			break;
		case MU_COMMAND_ICON:
			draw_icon(dev, cmd->icon.id, cmd->icon.rect,
				  cmd->icon.color);
			break;
		case MU_COMMAND_CLIP:
			buffer_flush(dev);
			glScissor((GLint)cmd->clip.rect.x,
				  height -
				      (cmd->clip.rect.y + cmd->clip.rect.h),
				  (GLsizei)cmd->clip.rect.w,
				  (GLsizei)cmd->clip.rect.h);
			break;
		default:
			break;
		}
	}
	buffer_flush(dev);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}
