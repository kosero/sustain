#include "renderer/renderer.h"

#include "core/core.h"
#include "core/log.h"
#include "ecs/components.h"
#include "renderer/glutil.h"
#include "renderer/mesh.h"
#include "renderer/shader.h"
#include "shaders_generated.h"
#include <cglm/struct.h>
#include <glad/gl.h>

enum {
	RENDERER_DEFAULT_WIDTH = 640,
	RENDERER_DEFAULT_HEIGHT = 480,
	RENDERER_SPHERE_RINGS = 16,
	RENDERER_SPHERE_SECTORS = 16,
	RENDERER_GRID_HALF_SIZE = 10,
};

#define RENDERER_GRID_SPACING 1.0f

static void renderer_create_framebuffer(RendererContext *rctx)
{
	glGenFramebuffers(1, &rctx->viewport_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rctx->viewport_fbo);

	glGenTextures(1, &rctx->viewport_color_tex);
	glBindTexture(GL_TEXTURE_2D, rctx->viewport_color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rctx->viewport_width,
		     rctx->viewport_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, rctx->viewport_color_tex, 0);

	glGenRenderbuffers(1, &rctx->viewport_depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rctx->viewport_depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
			      rctx->viewport_width, rctx->viewport_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				  GL_RENDERBUFFER, rctx->viewport_depth_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
	    GL_FRAMEBUFFER_COMPLETE) {
		log_printf(LOG_LEVEL_ERROR, "viewport framebuffer incomplete");
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void renderer_destroy_framebuffer(RendererContext *rctx)
{
	glDeleteFramebuffers(1, &rctx->viewport_fbo);
	glDeleteTextures(1, &rctx->viewport_color_tex);
	glDeleteRenderbuffers(1, &rctx->viewport_depth_rbo);
	rctx->viewport_fbo = 0;
	rctx->viewport_color_tex = 0;
	rctx->viewport_depth_rbo = 0;
}

static void renderer_create_blit_geometry(RendererContext *rctx)
{
	glGenVertexArrays(1, &rctx->blit_vao);
	glGenBuffers(1, &rctx->blit_vbo);

	glBindVertexArray(rctx->blit_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rctx->blit_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			      4 * (GLsizei)sizeof(float), NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			      4 * (GLsizei)sizeof(float),
			      gl_attrib_offset(2 * sizeof(float)));
	glBindVertexArray(0);
}

static void renderer_blit_quad(RendererContext *rctx, Rect dst, int screen_w,
			       int screen_h)
{
	float x = -1.0f + ((2.0f * dst.x) / (float)screen_w);
	float y = 1.0f - ((2.0f * (dst.y + dst.h)) / (float)screen_h);
	float w = (2.0f * dst.w) / (float)screen_w;
	float h = (2.0f * dst.h) / (float)screen_h;

	// two triangles; v=0 maps to the rect's bottom vertex. The FBO
	// texture's v=0 is the scene's bottom row, so this renders the image
	// upright.
	float verts[6][4] = {
	    {x, y, 0.0f, 0.0f},	    {x + w, y, 1.0f, 0.0f},
	    {x, y + h, 0.0f, 1.0f}, {x, y + h, 0.0f, 1.0f},
	    {x + w, y, 1.0f, 0.0f}, {x + w, y + h, 1.0f, 1.0f},
	};

	glBindVertexArray(rctx->blit_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rctx->blit_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts[0][0],
		     GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

static vec4s renderer_color_to_vec4s(Color color)
{
	return (vec4s){{(float)color.r / 255.0f, (float)color.g / 255.0f,
			(float)color.b / 255.0f, (float)color.a / 255.0f}};
}

void renderer_context_init(struct CoreContext *ctx)
{
	RendererContext *rctx = &ctx->renderer;

	rctx->primitive_shader =
	    gl_shader_create(shader_primitive_vert, shader_primitive_frag);
	rctx->grid_shader =
	    gl_shader_create(shader_grid_vert, shader_grid_frag);
	rctx->blit_shader =
	    gl_shader_create(shader_blit_vert, shader_blit_frag);

	rctx->prim_model_loc =
	    gl_shader_location(rctx->primitive_shader, "uModel");
	rctx->prim_view_loc =
	    gl_shader_location(rctx->primitive_shader, "uView");
	rctx->prim_proj_loc =
	    gl_shader_location(rctx->primitive_shader, "uProj");
	rctx->prim_color_loc =
	    gl_shader_location(rctx->primitive_shader, "uColor");
	rctx->prim_light_loc =
	    gl_shader_location(rctx->primitive_shader, "uLightDir");
	rctx->grid_view_loc = gl_shader_location(rctx->grid_shader, "uView");
	rctx->grid_proj_loc = gl_shader_location(rctx->grid_shader, "uProj");
	rctx->blit_tex_loc = gl_shader_location(rctx->blit_shader, "uTexture");

	gl_mesh_build_cube(&rctx->cube_mesh, &rctx->cube_wire_mesh);
	if (gl_mesh_build_sphere(&rctx->sphere_mesh, RENDERER_SPHERE_RINGS,
				 RENDERER_SPHERE_SECTORS) != 0) {
		log_printf(LOG_LEVEL_ERROR, "sphere mesh allocation failed");
	}
	if (gl_mesh_build_grid(&rctx->grid_mesh, RENDERER_GRID_HALF_SIZE,
			       RENDERER_GRID_SPACING) != 0) {
		log_printf(LOG_LEVEL_ERROR, "grid mesh allocation failed");
	}
	renderer_create_blit_geometry(rctx);

	rctx->viewport_width = RENDERER_DEFAULT_WIDTH;
	rctx->viewport_height = RENDERER_DEFAULT_HEIGHT;
	rctx->viewport_initialized = true;
	renderer_create_framebuffer(rctx);

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
		renderer_destroy_framebuffer(rctx);
	}

	rctx->viewport_width = width;
	rctx->viewport_height = height;
	rctx->viewport_initialized = true;
	renderer_create_framebuffer(rctx);
}

void renderer_draw_viewport(RendererContext *rctx, Camera3D *camera,
			    ecs_world_t *world)
{
	glBindFramebuffer(GL_FRAMEBUFFER, rctx->viewport_fbo);
	glViewport(0, 0, rctx->viewport_width, rctx->viewport_height);
	glClearColor(30.0f / 255.0f, 30.0f / 255.0f, 35.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	float aspect =
	    (float)rctx->viewport_width / (float)rctx->viewport_height;
	mat4s view = glms_lookat(camera->position, camera->target, camera->up);
	mat4s proj = glms_perspective(glm_rad(camera->fovy), aspect,
				      camera->znear, camera->zfar);

	glUseProgram(rctx->grid_shader.id);
	glUniformMatrix4fv(rctx->grid_view_loc, 1, GL_FALSE, &view.raw[0][0]);
	glUniformMatrix4fv(rctx->grid_proj_loc, 1, GL_FALSE, &proj.raw[0][0]);
	glBindVertexArray(rctx->grid_mesh.vao);
	glDrawElements(GL_LINES, rctx->grid_mesh.index_count, GL_UNSIGNED_INT,
		       NULL);

	glUseProgram(rctx->primitive_shader.id);
	glUniformMatrix4fv(rctx->prim_view_loc, 1, GL_FALSE, &view.raw[0][0]);
	glUniformMatrix4fv(rctx->prim_proj_loc, 1, GL_FALSE, &proj.raw[0][0]);

	vec3s light_world = glms_vec3_normalize((vec3s){{0.5f, 1.0f, 0.6f}});
	vec3s light_view =
	    glms_vec3_normalize(glms_mat4_mulv3(view, light_world, 0.0f));
	glUniform3f(rctx->prim_light_loc, light_view.x, light_view.y,
		    light_view.z);

	ecs_iter_t it = ecs_query_iter(world, rctx->render_query);
	while (ecs_query_next(&it)) {
		Transform3D *t = ecs_field(&it, Transform3D, 0);
		MeshRenderer *m = ecs_field(&it, MeshRenderer, 1);

		for (int i = 0; i < it.count; i++) {
			mat4s model = glms_mat4_identity();
			model = glms_translate(model, t[i].position);
			model = glms_rotate_y(model, glm_rad(t[i].rotation.y));
			model = glms_rotate_x(model, glm_rad(t[i].rotation.x));
			model = glms_rotate_z(model, glm_rad(t[i].rotation.z));
			model = glms_scale(model, t[i].scale);

			vec4s color = renderer_color_to_vec4s(m[i].color);

			if (m[i].type == PRIMITIVE_CUBE) {
				glUniformMatrix4fv(rctx->prim_model_loc, 1,
						   GL_FALSE, &model.raw[0][0]);
				glUniform4f(rctx->prim_color_loc, color.x,
					    color.y, color.z, color.w);
				glBindVertexArray(rctx->cube_mesh.vao);
				glDrawElements(GL_TRIANGLES,
					       rctx->cube_mesh.index_count,
					       GL_UNSIGNED_INT, NULL);

				glUniform4f(rctx->prim_color_loc, 0.0f, 0.0f,
					    0.0f, 1.0f);
				glBindVertexArray(rctx->cube_wire_mesh.vao);
				glDrawElements(GL_LINES,
					       rctx->cube_wire_mesh.index_count,
					       GL_UNSIGNED_INT, NULL);
			} else if (m[i].type == PRIMITIVE_SPHERE) {
				glUniformMatrix4fv(rctx->prim_model_loc, 1,
						   GL_FALSE, &model.raw[0][0]);
				glUniform4f(rctx->prim_color_loc, color.x,
					    color.y, color.z, color.w);
				glBindVertexArray(rctx->sphere_mesh.vao);
				glDrawElements(GL_TRIANGLES,
					       rctx->sphere_mesh.index_count,
					       GL_UNSIGNED_INT, NULL);
			}
		}
	}

	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer_blit_viewport(RendererContext *rctx, Rect dst, int screen_w,
			    int screen_h)
{
	if (!rctx->viewport_initialized) {
		return;
	}

	glViewport(0, 0, screen_w, screen_h);
	glUseProgram(rctx->blit_shader.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rctx->viewport_color_tex);
	glUniform1i(rctx->blit_tex_loc, 0);
	renderer_blit_quad(rctx, dst, screen_w, screen_h);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void renderer_context_cleanup(RendererContext *rctx)
{
	if (rctx->render_query != NULL) {
		ecs_query_fini(rctx->render_query);
		rctx->render_query = NULL;
	}

	gl_mesh_destroy(&rctx->cube_mesh);
	gl_mesh_destroy(&rctx->cube_wire_mesh);
	gl_mesh_destroy(&rctx->sphere_mesh);
	gl_mesh_destroy(&rctx->grid_mesh);

	glDeleteVertexArrays(1, &rctx->blit_vao);
	glDeleteBuffers(1, &rctx->blit_vbo);
	rctx->blit_vao = 0;
	rctx->blit_vbo = 0;

	gl_shader_destroy(&rctx->primitive_shader);
	gl_shader_destroy(&rctx->grid_shader);
	gl_shader_destroy(&rctx->blit_shader);

	if (rctx->viewport_initialized) {
		renderer_destroy_framebuffer(rctx);
		rctx->viewport_initialized = false;
	}
}
