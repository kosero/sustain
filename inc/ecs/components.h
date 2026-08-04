#ifndef SUSTAIN_COMPONENTS_H
#define SUSTAIN_COMPONENTS_H

#include "math/math.h"
#include <cglm/struct.h>
#include <flecs.h>

typedef struct {
	vec3s position;
	vec3s rotation;
	vec3s scale;
} Transform3D;

typedef enum { PRIMITIVE_CUBE, PRIMITIVE_SPHERE } PrimitiveType;

typedef struct {
	PrimitiveType type;
	Color color;
} MeshRenderer;

// NOLINTBEGIN
extern ECS_COMPONENT_DECLARE(Transform3D);
extern ECS_COMPONENT_DECLARE(MeshRenderer);
extern ECS_TAG_DECLARE(GameObject);
// NOLINTEND

void components_register(ecs_world_t *world);

#endif // SUSTAIN_COMPONENTS_H
