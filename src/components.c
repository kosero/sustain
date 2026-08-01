#include "components.h"

// NOLINTBEGIN
ECS_COMPONENT_DECLARE(Transform3D);
ECS_COMPONENT_DECLARE(MeshRenderer);
// NOLINTEND

void components_register(ecs_world_t *world) {
  ECS_COMPONENT_DEFINE(world, Transform3D);
  ECS_COMPONENT_DEFINE(world, MeshRenderer);
}
