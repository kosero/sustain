#include "renderer/shader.h"

#include "core/log.h"
#include <stdlib.h>

GLuint gl_shader_compile_stage(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE) {
		return shader;
	}

	GLint log_len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
	char *log = (char *)malloc((size_t)log_len + 1);
	if (log != NULL) {
		glGetShaderInfoLog(shader, log_len, NULL, log);
		log_printf(LOG_LEVEL_ERROR, "shader compile error: %s", log);
		free(log);
	}
	glDeleteShader(shader);
	return 0;
}

GLShader gl_shader_create(const char *vertex_src, const char *fragment_src)
{
	GLShader shader = {0};

	GLuint vs = gl_shader_compile_stage(GL_VERTEX_SHADER, vertex_src);
	GLuint fs = gl_shader_compile_stage(GL_FRAGMENT_SHADER, fragment_src);
	if (vs == 0 || fs == 0) {
		if (vs != 0) {
			glDeleteShader(vs);
		}
		if (fs != 0) {
			glDeleteShader(fs);
		}
		return shader;
	}

	shader.id = glCreateProgram();
	glAttachShader(shader.id, vs);
	glAttachShader(shader.id, fs);
	glLinkProgram(shader.id);
	glDeleteShader(vs);
	glDeleteShader(fs);

	GLint status = 0;
	glGetProgramiv(shader.id, GL_LINK_STATUS, &status);
	if (status == GL_TRUE) {
		return shader;
	}

	GLint log_len = 0;
	glGetProgramiv(shader.id, GL_INFO_LOG_LENGTH, &log_len);
	char *log = (char *)malloc((size_t)log_len + 1);
	if (log != NULL) {
		glGetProgramInfoLog(shader.id, log_len, NULL, log);
		log_printf(LOG_LEVEL_ERROR, "shader link error: %s", log);
		free(log);
	}
	glDeleteProgram(shader.id);
	shader.id = 0;
	return shader;
}

GLint gl_shader_location(GLShader shader, const char *name)
{
	return glGetUniformLocation(shader.id, name);
}

void gl_shader_destroy(GLShader *shader)
{
	if (shader->id != 0) {
		glDeleteProgram(shader->id);
		shader->id = 0;
	}
}
