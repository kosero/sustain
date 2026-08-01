#ifndef SUSTAIN_EDITOR_CAMERA_H
#define SUSTAIN_EDITOR_CAMERA_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
	Camera3D camera;
	bool is_moving;
} EditorCamera;

void editor_camera_init(EditorCamera *cam);
void editor_camera_update(EditorCamera *cam, bool is_hovered);

#endif // SUSTAIN_EDITOR_CAMERA_H
