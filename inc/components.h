#ifndef SUSTAIN_COMPONENTS_H
#define SUSTAIN_COMPONENTS_H

#include "raylib.h"
#include <flecs.h>

typedef struct {
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
} Transform3D;

typedef enum { PRIMITIVE_CUBE, PRIMITIVE_SPHERE } PrimitiveType;

typedef struct {
	PrimitiveType type;
	Color color;
} MeshRenderer;

extern ECS_COMPONENT_DECLARE(Transform3D);
extern ECS_COMPONENT_DECLARE(MeshRenderer);
extern ECS_TAG_DECLARE(GameObject);

void components_register(ecs_world_t *world);

#endif // SUSTAIN_COMPONENTS_H
