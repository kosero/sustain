#include "renderer/mesh.h"

#include "renderer/glutil.h"
#include <math.h>
#include <stdlib.h>

#define GL_MESH_PI 3.14159265358979323846f

static inline GLsizei get_format_stride(GLMeshFormat format)
{
	return (format == GL_MESH_FORMAT_POS) ? 3 : 6;
}

void gl_mesh_create(GLMesh *mesh, GLMeshFormat format, const float *vertices,
		    size_t vertex_count, const unsigned int *indices,
		    size_t index_count, GLenum usage)
{
	GLsizei stride = get_format_stride(format);
	mesh->usage = usage;
	mesh->vbo_capacity_bytes = vertex_count * stride * sizeof(float);
	mesh->ebo_capacity_bytes = index_count * sizeof(unsigned int);

	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)mesh->vbo_capacity_bytes,
		     vertices, usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		     (GLsizeiptr)mesh->ebo_capacity_bytes, indices, usage);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			      stride * (GLsizei)sizeof(float), (void *)0);

	if (format == GL_MESH_FORMAT_POS_EXT) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
				      stride * (GLsizei)sizeof(float),
				      gl_attrib_offset(3 * sizeof(float)));
	}

	glBindVertexArray(0);

	mesh->index_count = (GLsizei)index_count;
}

void gl_mesh_update(GLMesh *mesh, GLMeshFormat format, const float *vertices,
		    size_t vertex_count, const unsigned int *indices,
		    size_t index_count)
{
	if (!mesh || mesh->vao == 0) {
		return;
	}

	GLsizei stride = get_format_stride(format);
	size_t required_vbo_bytes = vertex_count * stride * sizeof(float);
	size_t required_ebo_bytes = index_count * sizeof(unsigned int);

	glBindVertexArray(mesh->vao);

	if (vertices && required_vbo_bytes > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		if (required_vbo_bytes > mesh->vbo_capacity_bytes) {
			glBufferData(GL_ARRAY_BUFFER,
				     (GLsizeiptr)required_vbo_bytes, vertices,
				     mesh->usage);
			mesh->vbo_capacity_bytes = required_vbo_bytes;
		} else {
			glBufferSubData(GL_ARRAY_BUFFER, 0,
					(GLsizeiptr)required_vbo_bytes,
					vertices);
		}
	}

	if (indices && required_ebo_bytes > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
		if (required_ebo_bytes > mesh->ebo_capacity_bytes) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				     (GLsizeiptr)required_ebo_bytes, indices,
				     mesh->usage);
			mesh->ebo_capacity_bytes = required_ebo_bytes;
		} else {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
					(GLsizeiptr)required_ebo_bytes,
					indices);
		}
		mesh->index_count = (GLsizei)index_count;
	}
	glBindVertexArray(0);
}

void gl_mesh_destroy(GLMesh *mesh)
{
	if (!mesh) {
		return;
	}
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
	mesh->vao = 0;
	mesh->vbo = 0;
	mesh->ebo = 0;
	mesh->index_count = 0;
	mesh->vbo_capacity_bytes = 0;
	mesh->ebo_capacity_bytes = 0;
}

void gl_mesh_build_cube(GLMesh *solid)
{
	const float corners[8][3] = {
	    {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
	    {-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
	    {1.0f, 1.0f, 1.0f},	   {-1.0f, 1.0f, 1.0f},
	};

	const struct CubeFace {
		float n[3];
		int c[4];
	} faces[6] = {
	    {{0.0f, 0.0f, 1.0f}, {5, 6, 7, 4}},
	    {{0.0f, 0.0f, -1.0f}, {0, 3, 2, 1}},
	    {{1.0f, 0.0f, 0.0f}, {5, 1, 2, 6}},
	    {{-1.0f, 0.0f, 0.0f}, {0, 4, 7, 3}},
	    {{0.0f, 1.0f, 0.0f}, {3, 7, 6, 2}},
	    {{0.0f, -1.0f, 0.0f}, {4, 0, 1, 5}},
	};

	float verts[24][6];
	unsigned int indices[36];

	for (int f = 0; f < 6; f++) {
		for (int i = 0; i < 4; i++) {
			const float *c = corners[faces[f].c[i]];
			verts[(f * 4) + i][0] = c[0];
			verts[(f * 4) + i][1] = c[1];
			verts[(f * 4) + i][2] = c[2];
			verts[(f * 4) + i][3] = faces[f].n[0];
			verts[(f * 4) + i][4] = faces[f].n[1];
			verts[(f * 4) + i][5] = faces[f].n[2];
		}
		int base = f * 4;
		indices[(f * 6) + 0] = (unsigned int)base;
		indices[(f * 6) + 1] = (unsigned int)(base + 1);
		indices[(f * 6) + 2] = (unsigned int)(base + 2);
		indices[(f * 6) + 3] = (unsigned int)base;
		indices[(f * 6) + 4] = (unsigned int)(base + 2);
		indices[(f * 6) + 5] = (unsigned int)(base + 3);
	}

	gl_mesh_create(solid, GL_MESH_FORMAT_POS_EXT, &verts[0][0], 24, indices,
		       36, GL_STATIC_DRAW);
}

int gl_mesh_build_sphere(GLMesh *mesh, int rings, int sectors)
{
	size_t vertex_count = (size_t)(rings + 1) * (size_t)(sectors + 1);
	size_t index_count = (size_t)rings * (size_t)sectors * 6;

	float *verts = (float *)malloc(vertex_count * 6 * sizeof(float));
	unsigned int *indices =
	    (unsigned int *)malloc(index_count * sizeof(unsigned int));
	if (verts == NULL || indices == NULL) {
		free(verts);
		free(indices);
		return -1;
	}

	size_t v = 0;
	for (int r = 0; r <= rings; r++) {
		float theta = GL_MESH_PI * (float)r / (float)rings;
		float st = sinf(theta);
		float ct = cosf(theta);
		for (int s = 0; s <= sectors; s++) {
			float phi =
			    2.0f * GL_MESH_PI * (float)s / (float)sectors;
			float x = st * cosf(phi);
			float y = ct;
			float z = st * sinf(phi);
			verts[(v * 6) + 0] = x;
			verts[(v * 6) + 1] = y;
			verts[(v * 6) + 2] = z;
			verts[(v * 6) + 3] = x;
			verts[(v * 6) + 4] = y;
			verts[(v * 6) + 5] = z;
			v++;
		}
	}

	size_t idx = 0;
	for (int r = 0; r < rings; r++) {
		for (int s = 0; s < sectors; s++) {
			int a = (r * (sectors + 1)) + s;
			int b = a + sectors + 1;
			indices[idx++] = (unsigned int)a;
			indices[idx++] = (unsigned int)(a + 1);
			indices[idx++] = (unsigned int)b;
			indices[idx++] = (unsigned int)(a + 1);
			indices[idx++] = (unsigned int)(b + 1);
			indices[idx++] = (unsigned int)b;
		}
	}

	gl_mesh_create(mesh, GL_MESH_FORMAT_POS_EXT, verts, vertex_count,
		       indices, index_count, GL_STATIC_DRAW);
	free(verts);
	free(indices);
	return 0;
}

int gl_mesh_build_grid(GLMesh *mesh, int half_size, float spacing)
{
	int lines_per_axis = (half_size * 2) + 1;
	size_t vertex_count = (size_t)lines_per_axis * 4;
	size_t index_count = vertex_count;

	float *verts = (float *)malloc(vertex_count * 6 * sizeof(float));
	unsigned int *indices =
	    (unsigned int *)malloc(index_count * sizeof(unsigned int));
	if (verts == NULL || indices == NULL) {
		free(verts);
		free(indices);
		return -1;
	}

	size_t v = 0;
	size_t idx = 0;
	for (int i = -half_size; i <= half_size; i++) {
		float coord = spacing * (float)i;
		float c = (i == 0) ? 0.95f : 0.7f;

		verts[(v * 6) + 0] = -spacing * (float)half_size;
		verts[(v * 6) + 1] = 0.0f;
		verts[(v * 6) + 2] = coord;
		verts[(v * 6) + 3] = c;
		verts[(v * 6) + 4] = c;
		verts[(v * 6) + 5] = c;
		verts[((v + 1) * 6) + 0] = spacing * (float)half_size;
		verts[((v + 1) * 6) + 1] = 0.0f;
		verts[((v + 1) * 6) + 2] = coord;
		verts[((v + 1) * 6) + 3] = c;
		verts[((v + 1) * 6) + 4] = c;
		verts[((v + 1) * 6) + 5] = c;
		indices[idx++] = (unsigned int)v;
		indices[idx++] = (unsigned int)(v + 1);
		v += 2;

		verts[(v * 6) + 0] = coord;
		verts[(v * 6) + 1] = 0.0f;
		verts[(v * 6) + 2] = -spacing * (float)half_size;
		verts[(v * 6) + 3] = c;
		verts[(v * 6) + 4] = c;
		verts[(v * 6) + 5] = c;
		verts[((v + 1) * 6) + 0] = coord;
		verts[((v + 1) * 6) + 1] = 0.0f;
		verts[((v + 1) * 6) + 2] = spacing * (float)half_size;
		verts[((v + 1) * 6) + 3] = c;
		verts[((v + 1) * 6) + 4] = c;
		verts[((v + 1) * 6) + 5] = c;
		indices[idx++] = (unsigned int)v;
		indices[idx++] = (unsigned int)(v + 1);
		v += 2;
	}

	gl_mesh_create(mesh, GL_MESH_FORMAT_POS_EXT, verts, vertex_count,
		       indices, index_count, GL_STATIC_DRAW);
	free(verts);
	free(indices);
	return 0;
}
