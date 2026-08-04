#include "renderer/mesh.h"

#include "renderer/glutil.h"
#include <math.h>
#include <stdlib.h>

#define GL_MESH_PI 3.14159265358979323846f

void gl_mesh_create(GLMesh *mesh, GLMeshFormat format, const float *vertices,
		    size_t vertex_count, const unsigned int *indices,
		    size_t index_count)
{
	GLsizei stride = (format == GL_MESH_FORMAT_POS) ? 3 : 6;

	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER,
		     (GLsizeiptr)(vertex_count * stride * sizeof(float)),
		     vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		     (GLsizeiptr)(index_count * sizeof(unsigned int)), indices,
		     GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			      stride * (GLsizei)sizeof(float), NULL);

	if (format == GL_MESH_FORMAT_POS_EXT) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
				      stride * (GLsizei)sizeof(float),
				      gl_attrib_offset(3 * sizeof(float)));
	}

	glBindVertexArray(0);

	mesh->index_count = (GLsizei)index_count;
}

void gl_mesh_destroy(GLMesh *mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
	mesh->vao = 0;
	mesh->vbo = 0;
	mesh->ebo = 0;
	mesh->index_count = 0;
}

void gl_mesh_build_cube(GLMesh *solid, GLMesh *wire)
{
	// cube corners
	const float corners[8][3] = {
	    {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
	    {-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
	    {1.0f, 1.0f, 1.0f},	   {-1.0f, 1.0f, 1.0f},
	};

	// faces: normal + 4 corners (CCW from outside)
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
		       36);

	const int wire_edges[12][2] = {
	    {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
	    {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7},
	};

	float wire_verts[8][3];
	unsigned int wire_indices[24];

	for (int i = 0; i < 8; i++) {
		wire_verts[i][0] = corners[i][0];
		wire_verts[i][1] = corners[i][1];
		wire_verts[i][2] = corners[i][2];
	}
	for (int e = 0; e < 12; e++) {
		wire_indices[(e * 2) + 0] = (unsigned int)wire_edges[e][0];
		wire_indices[(e * 2) + 1] = (unsigned int)wire_edges[e][1];
	}

	gl_mesh_create(wire, GL_MESH_FORMAT_POS, &wire_verts[0][0], 8,
		       wire_indices, 24);
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
		       indices, index_count);
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
		       indices, index_count);
	free(verts);
	free(indices);
	return 0;
}
