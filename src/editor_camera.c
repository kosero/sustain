#include "editor_camera.h"
#include "raymath.h"
#include "rcamera.h"

void editor_camera_init(EditorCamera *cam) {
  cam->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
  cam->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  cam->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  cam->camera.fovy = 60.0f;
  cam->camera.projection = CAMERA_PERSPECTIVE;
  cam->is_moving = false;
}

void editor_camera_update(EditorCamera *cam, bool is_hovered) {
  if (is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    cam->is_moving = true;
    DisableCursor();
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    if (cam->is_moving) {
      cam->is_moving = false;
      EnableCursor();
    }
  }

  if (cam->is_moving) {
    float move_speed = 10.0f * GetFrameTime();
    
    // false = move in look direction instead of world plane
    if (IsKeyDown(KEY_W)) { CameraMoveForward(&cam->camera, move_speed, false); }
    if (IsKeyDown(KEY_S)) { CameraMoveForward(&cam->camera, -move_speed, false); }
    if (IsKeyDown(KEY_D)) { CameraMoveRight(&cam->camera, move_speed, false); }
    if (IsKeyDown(KEY_A)) { CameraMoveRight(&cam->camera, -move_speed, false); }
    if (IsKeyDown(KEY_E)) { CameraMoveUp(&cam->camera, move_speed); }
    if (IsKeyDown(KEY_Q)) { CameraMoveUp(&cam->camera, -move_speed); }

    Vector2 mouse_delta = GetMouseDelta();
    CameraYaw(&cam->camera, -mouse_delta.x * 0.003f, false);
    CameraPitch(&cam->camera, -mouse_delta.y * 0.003f, true, false, false);
  }
}
