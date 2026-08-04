#ifndef SUSTAIN_MESH_H
#define SUSTAIN_MESH_H

#include <glad/gl.h>
#include <stddef.h>

typedef enum {
	GL_MESH_FORMAT_POS,
	GL_MESH_FORMAT_POS_EXT,
} GLMeshFormat;

typedef struct {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLsizei index_count;
} GLMesh;

void gl_mesh_create(GLMesh *mesh, GLMeshFormat format, const float *vertices,
		    size_t vertex_count, const unsigned int *indices,
		    size_t index_count);
void gl_mesh_destroy(GLMesh *mesh);

void gl_mesh_build_cube(GLMesh *solid);
int gl_mesh_build_sphere(GLMesh *mesh, int rings, int sectors);
int gl_mesh_build_grid(GLMesh *mesh, int half_size, float spacing);

#endif // SUSTAIN_MESH_H
