#include "core/input.h"
#include "core/core.h"
#include "core/log.h"

#define INPUT_NUM_KEYS SDL_SCANCODE_COUNT
enum { INPUT_NUM_MOUSE_BUTTONS = 5 };

typedef struct InputState {
	bool keys_now[INPUT_NUM_KEYS];
	bool keys_prev[INPUT_NUM_KEYS];
	bool mouse_buttons_now[INPUT_NUM_MOUSE_BUTTONS];
	bool mouse_buttons_prev[INPUT_NUM_MOUSE_BUTTONS];
	float mouse_x;
	float mouse_y;
	float mouse_delta_x;
	float mouse_delta_y;
	bool window_resized;
	int window_width;
	int window_height;
	bool quit_requested;
} InputState;

static InputState *get_input_state_internal(void)
{
	static InputState state;
	return &state;
}

#define G_INPUT get_input_state_internal()

void input_frame_begin(void)
{
	for (int i = 0; i < INPUT_NUM_KEYS; i++) {
		G_INPUT->keys_prev[i] = G_INPUT->keys_now[i];
	}
	for (int i = 0; i < INPUT_NUM_MOUSE_BUTTONS; i++) {
		G_INPUT->mouse_buttons_prev[i] = G_INPUT->mouse_buttons_now[i];
	}

	G_INPUT->mouse_delta_x = 0.0f;
	G_INPUT->mouse_delta_y = 0.0f;
	G_INPUT->window_resized = false;
}
void input_handle_event(const SDL_Event *evt)
{
	switch (evt->type) {
	case SDL_EVENT_QUIT:
		G_INPUT->quit_requested = true;
		break;
	case SDL_EVENT_KEY_DOWN:
		if (!evt->key.repeat && evt->key.scancode < INPUT_NUM_KEYS) {
			G_INPUT->keys_now[evt->key.scancode] = true;
		}
		break;
	case SDL_EVENT_KEY_UP:
		if (evt->key.scancode < INPUT_NUM_KEYS) {
			G_INPUT->keys_now[evt->key.scancode] = false;
		}
		break;
	case SDL_EVENT_MOUSE_MOTION:
		G_INPUT->mouse_x = evt->motion.x;
		G_INPUT->mouse_y = evt->motion.y;
		G_INPUT->mouse_delta_x += evt->motion.xrel;
		G_INPUT->mouse_delta_y += evt->motion.yrel;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (evt->button.button >= 1 &&
		    evt->button.button <= INPUT_NUM_MOUSE_BUTTONS) {
			G_INPUT->mouse_buttons_now[evt->button.button - 1] =
			    true;
			G_INPUT->mouse_x = evt->button.x;
			G_INPUT->mouse_y = evt->button.y;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		if (evt->button.button >= 1 &&
		    evt->button.button <= INPUT_NUM_MOUSE_BUTTONS) {
			G_INPUT->mouse_buttons_now[evt->button.button - 1] =
			    false;
			G_INPUT->mouse_x = evt->button.x;
			G_INPUT->mouse_y = evt->button.y;
		}
		break;
	case SDL_EVENT_WINDOW_RESIZED:
		G_INPUT->window_resized = true;
		G_INPUT->window_width = evt->window.data1;
		G_INPUT->window_height = evt->window.data2;
		break;
	default:
		break;
	}
}

bool input_key_down(SDL_Scancode scancode)
{
	if (scancode >= INPUT_NUM_KEYS) {
		return false;
	}
	return G_INPUT->keys_now[scancode];
}

bool input_key_pressed(SDL_Scancode scancode)
{
	if (scancode >= INPUT_NUM_KEYS) {
		return false;
	}
	return (bool)(G_INPUT->keys_now[scancode] &&
		      !G_INPUT->keys_prev[scancode]);
}

bool input_mouse_button_pressed(int button)
{
	if (button < 1 || button > INPUT_NUM_MOUSE_BUTTONS) {
		return false;
	}
	return (bool)(G_INPUT->mouse_buttons_now[button - 1] &&
		      !G_INPUT->mouse_buttons_prev[button - 1]);
}

bool input_mouse_button_released(int button)
{
	if (button < 1 || button > INPUT_NUM_MOUSE_BUTTONS) {
		return false;
	}
	return (bool)(!G_INPUT->mouse_buttons_now[button - 1] &&
		      G_INPUT->mouse_buttons_prev[button - 1]);
}

float input_mouse_x(void) { return G_INPUT->mouse_x; }

float input_mouse_y(void) { return G_INPUT->mouse_y; }

float input_mouse_delta_x(void) { return G_INPUT->mouse_delta_x; }

float input_mouse_delta_y(void) { return G_INPUT->mouse_delta_y; }

bool input_window_resized(void) { return G_INPUT->window_resized; }

int input_window_width(void) { return G_INPUT->window_width; }

int input_window_height(void) { return G_INPUT->window_height; }

bool input_quit_requested(void) { return G_INPUT->quit_requested; }

void input_set_cursor_grabbed(bool grabbed)
{
	if (SDL_SetWindowRelativeMouseMode(core_get_window(), grabbed)) {
		return;
	}
	log_printf(LOG_LEVEL_WARN, "could not set relative mouse mode: %s",
		   SDL_GetError());
}
