#ifndef SUSTAIN_RENDERER_H
#define SUSTAIN_RENDERER_H

#include "math/camera.h"
#include "math/math.h"
#include "renderer/mesh.h"
#include "renderer/shader.h"
#include <flecs.h>
#include <stdbool.h>
struct CoreContext;

typedef struct {
	GLuint viewport_fbo;
	GLuint viewport_color_tex;
	GLuint viewport_depth_rbo;
	int viewport_width;
	int viewport_height;
	bool viewport_initialized;
	Rect viewport_draw_rect;
	ecs_query_t *render_query;

	GLShader primitive_shader;
	GLShader grid_shader;
	GLShader blit_shader;

	GLMesh cube_mesh;
	GLMesh cube_wire_mesh;
	GLMesh sphere_mesh;

	GLuint blit_vao;
	GLuint blit_vbo;
	GLuint grid_vao;
	GLuint grid_vbo;
	GLint prim_model_loc;
	GLint prim_view_loc;
	GLint prim_proj_loc;
	GLint prim_color_loc;
	GLint prim_light_loc;
	GLint grid_inv_vp_loc;
	GLint grid_cam_pos_loc;
	GLint blit_tex_loc;
} RendererContext;

void renderer_context_init(struct CoreContext *ctx);
void renderer_viewport_resize(RendererContext *rctx, int width, int height);
void renderer_draw_viewport(RendererContext *rctx, Camera3D *camera,
			    ecs_world_t *world);
void renderer_blit_viewport(RendererContext *rctx, Rect dst, int screen_w,
			    int screen_h);
void renderer_context_cleanup(RendererContext *rctx);

#endif // SUSTAIN_RENDERER_H
