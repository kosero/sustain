#ifndef SUSTAIN_SHADER_H
#define SUSTAIN_SHADER_H

#include <glad/gl.h>

typedef struct {
	GLuint id;
} GLShader;

GLShader gl_shader_create(const char *vertex_src, const char *fragment_src);
GLuint gl_shader_compile_stage(GLenum type, const char *source);
GLint gl_shader_location(GLShader shader, const char *name);
void gl_shader_destroy(GLShader *shader);

#endif // SUSTAIN_SHADER_H
