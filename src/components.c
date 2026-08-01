#include "components.h"

// NOLINTBEGIN
ECS_COMPONENT_DECLARE(Transform3D);
ECS_COMPONENT_DECLARE(MeshRenderer);
ECS_TAG_DECLARE(GameObject);
// NOLINTEND

void components_register(ecs_world_t *world)
{
	ECS_COMPONENT_DEFINE(world, Transform3D);
	ECS_COMPONENT_DEFINE(world, MeshRenderer);
	ECS_TAG_DEFINE(world, GameObject);
}
