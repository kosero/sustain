#ifndef SUSTAIN_RENDERER_H
#define SUSTAIN_RENDERER_H

#include "cglm/mat4.h"
#include "cglm/vec2.h"
#include "sustain.h"

typedef struct {
	mat4 model;
	mat4 view;
	mat4 proj;
} UniformBufferObject;

typedef struct Vertex {
	vec2 pos;
	Color color;
} Vertex;

#endif // SUSTAIN_RENDERER_H

