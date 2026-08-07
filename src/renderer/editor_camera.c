#include "renderer/editor_camera.h"

#include "core/core.h"
#include "core/input.h"

#define CAMERA_MOUSE_SENSITIVITY 0.003f
#define CAMERA_PITCH_LIMIT (GLM_PI_2f - 0.01f)

static vec3s editor_camera_forward(EditorCamera *cam)
{
	return (vec3s){{cosf(cam->pitch) * sinf(cam->yaw), sinf(cam->pitch),
			-cosf(cam->pitch) * cosf(cam->yaw)}};
}

static vec3s editor_camera_right(EditorCamera *cam)
{
	return (vec3s){{cosf(cam->yaw), 0.0f, sinf(cam->yaw)}};
}

static void editor_camera_recompute_target(EditorCamera *cam)
{
	cam->camera.target =
	    glms_vec3_add(cam->camera.position, editor_camera_forward(cam));
}

static vec3s editor_camera_keyboard_movement(vec3s forward, vec3s right)
{
	vec3s movement = {{0.0f, 0.0f, 0.0f}};
	if (input_key_down(SDL_SCANCODE_W)) {
		movement = glms_vec3_add(movement, forward);
	}
	if (input_key_down(SDL_SCANCODE_S)) {
		movement = glms_vec3_sub(movement, forward);
	}
	if (input_key_down(SDL_SCANCODE_D)) {
		movement = glms_vec3_add(movement, right);
	}
	if (input_key_down(SDL_SCANCODE_A)) {
		movement = glms_vec3_sub(movement, right);
	}
	if (input_key_down(SDL_SCANCODE_E)) {
		movement.raw[1] += 1.0f;
	}
	if (input_key_down(SDL_SCANCODE_Q)) {
		movement.raw[1] -= 1.0f;
	}
	return movement;
}

static void editor_camera_apply_movement(EditorCamera *cam, float move_speed)
{
	vec3s forward = editor_camera_forward(cam);
	vec3s right = editor_camera_right(cam);
	vec3s movement = editor_camera_keyboard_movement(forward, right);
	cam->camera.position = glms_vec3_add(
	    cam->camera.position, glms_vec3_scale(movement, move_speed));
}

void editor_camera_init(EditorCamera *cam)
{
	cam->camera.position = (vec3s){{10.0f, 10.0f, 10.0f}};
	cam->camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	cam->camera.fovy = 60.0f;
	cam->camera.znear = 0.01f;
	cam->camera.zfar = 1000.0f;
	cam->is_moving = false;

	vec3s forward = glms_vec3_normalize(
	    glms_vec3_sub((vec3s){{0.0f, 0.0f, 0.0f}}, cam->camera.position));
	cam->pitch = asinf(forward.raw[1]);
	cam->yaw = atan2f(forward.raw[0], -forward.raw[2]);
	editor_camera_recompute_target(cam);
}

void editor_camera_update(EditorCamera *cam, bool is_hovered)
{
	if (is_hovered && input_mouse_button_pressed(SDL_BUTTON_RIGHT)) {
		cam->is_moving = true;
		input_set_cursor_grabbed(true);
	}

	if (input_mouse_button_released(SDL_BUTTON_RIGHT)) {
		if (cam->is_moving) {
			cam->is_moving = false;
			input_set_cursor_grabbed(false);
		}
	}

	if (cam->is_moving) {
		editor_camera_apply_movement(cam,
					     10.0f * core_get_delta_time());

		cam->yaw += input_mouse_delta_x() * CAMERA_MOUSE_SENSITIVITY;
		cam->pitch -= input_mouse_delta_y() * CAMERA_MOUSE_SENSITIVITY;

		if (cam->pitch > CAMERA_PITCH_LIMIT) {
			cam->pitch = CAMERA_PITCH_LIMIT;
		}
		if (cam->pitch < -CAMERA_PITCH_LIMIT) {
			cam->pitch = -CAMERA_PITCH_LIMIT;
		}

		editor_camera_recompute_target(cam);
	}
}
