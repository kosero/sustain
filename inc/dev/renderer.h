#ifndef SUSTAIN_RENDERER_H
#define SUSTAIN_RENDERER_H

#include "sustain.h"

typedef struct {
	Mat4 model;
	Mat4 view;
	Mat4 proj;
} UniformBufferObject;

typedef struct Vertex {
	Vec2 pos;
	Color color;
} Vertex;

#endif // SUSTAIN_RENDERER_H
