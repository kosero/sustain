#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"
#include "sustain.h"
#include <string.h>

static const SDL_Scancode key_scancode[KEY_COUNT] = {
    [KEY_A] = SDL_SCANCODE_A,
    [KEY_B] = SDL_SCANCODE_B,
    [KEY_C] = SDL_SCANCODE_C,
    [KEY_D] = SDL_SCANCODE_D,
    [KEY_E] = SDL_SCANCODE_E,
    [KEY_F] = SDL_SCANCODE_F,
    [KEY_G] = SDL_SCANCODE_G,
    [KEY_H] = SDL_SCANCODE_H,
    [KEY_I] = SDL_SCANCODE_I,
    [KEY_J] = SDL_SCANCODE_J,
    [KEY_K] = SDL_SCANCODE_K,
    [KEY_L] = SDL_SCANCODE_L,
    [KEY_M] = SDL_SCANCODE_M,
    [KEY_N] = SDL_SCANCODE_N,
    [KEY_O] = SDL_SCANCODE_O,
    [KEY_P] = SDL_SCANCODE_P,
    [KEY_Q] = SDL_SCANCODE_Q,
    [KEY_R] = SDL_SCANCODE_R,
    [KEY_S] = SDL_SCANCODE_S,
    [KEY_T] = SDL_SCANCODE_T,
    [KEY_U] = SDL_SCANCODE_U,
    [KEY_V] = SDL_SCANCODE_V,
    [KEY_W] = SDL_SCANCODE_W,
    [KEY_X] = SDL_SCANCODE_X,
    [KEY_Y] = SDL_SCANCODE_Y,
    [KEY_Z] = SDL_SCANCODE_Z,

    [KEY_ZERO] = SDL_SCANCODE_0,
    [KEY_ONE] = SDL_SCANCODE_1,
    [KEY_TWO] = SDL_SCANCODE_2,
    [KEY_THREE] = SDL_SCANCODE_3,
    [KEY_FOUR] = SDL_SCANCODE_4,
    [KEY_FIVE] = SDL_SCANCODE_5,
    [KEY_SIX] = SDL_SCANCODE_6,
    [KEY_SEVEN] = SDL_SCANCODE_7,
    [KEY_EIGHT] = SDL_SCANCODE_8,
    [KEY_NINE] = SDL_SCANCODE_9,

    [KEY_F1] = SDL_SCANCODE_F1,
    [KEY_F2] = SDL_SCANCODE_F2,
    [KEY_F3] = SDL_SCANCODE_F3,
    [KEY_F4] = SDL_SCANCODE_F4,
    [KEY_F5] = SDL_SCANCODE_F5,
    [KEY_F6] = SDL_SCANCODE_F6,
    [KEY_F7] = SDL_SCANCODE_F7,
    [KEY_F8] = SDL_SCANCODE_F8,
    [KEY_F9] = SDL_SCANCODE_F9,
    [KEY_F10] = SDL_SCANCODE_F10,
    [KEY_F11] = SDL_SCANCODE_F11,
    [KEY_F12] = SDL_SCANCODE_F12,

    [KEY_SPACE] = SDL_SCANCODE_SPACE,
    [KEY_ENTER] = SDL_SCANCODE_RETURN,
    [KEY_ESCAPE] = SDL_SCANCODE_ESCAPE,
    [KEY_TAB] = SDL_SCANCODE_TAB,
    [KEY_BACKSPACE] = SDL_SCANCODE_BACKSPACE,
    [KEY_DELETE] = SDL_SCANCODE_DELETE,
    [KEY_INSERT] = SDL_SCANCODE_INSERT,
    [KEY_HOME] = SDL_SCANCODE_HOME,
    [KEY_END] = SDL_SCANCODE_END,
    [KEY_PAGE_UP] = SDL_SCANCODE_PAGEUP,
    [KEY_PAGE_DOWN] = SDL_SCANCODE_PAGEDOWN,

    [KEY_LEFT] = SDL_SCANCODE_LEFT,
    [KEY_RIGHT] = SDL_SCANCODE_RIGHT,
    [KEY_UP] = SDL_SCANCODE_UP,
    [KEY_DOWN] = SDL_SCANCODE_DOWN,

    [KEY_LEFT_SHIFT] = SDL_SCANCODE_LSHIFT,
    [KEY_RIGHT_SHIFT] = SDL_SCANCODE_RSHIFT,
    [KEY_LEFT_CTRL] = SDL_SCANCODE_LCTRL,
    [KEY_RIGHT_CTRL] = SDL_SCANCODE_RCTRL,
    [KEY_LEFT_ALT] = SDL_SCANCODE_LALT,
    [KEY_RIGHT_ALT] = SDL_SCANCODE_RALT,
    [KEY_LEFT_SUPER] = SDL_SCANCODE_LGUI,
    [KEY_RIGHT_SUPER] = SDL_SCANCODE_RGUI,

    [KEY_MINUS] = SDL_SCANCODE_MINUS,
    [KEY_EQUALS] = SDL_SCANCODE_EQUALS,
    [KEY_COMMA] = SDL_SCANCODE_COMMA,
    [KEY_PERIOD] = SDL_SCANCODE_PERIOD,
    [KEY_SLASH] = SDL_SCANCODE_SLASH,
    [KEY_SEMICOLON] = SDL_SCANCODE_SEMICOLON,
    [KEY_APOSTROPHE] = SDL_SCANCODE_APOSTROPHE,
    [KEY_LEFT_BRACKET] = SDL_SCANCODE_LEFTBRACKET,
    [KEY_RIGHT_BRACKET] = SDL_SCANCODE_RIGHTBRACKET,
    [KEY_BACKSLASH] = SDL_SCANCODE_BACKSLASH,
    [KEY_GRAVE] = SDL_SCANCODE_GRAVE,
};

typedef struct Input_Context {
	uint8_t key_down[SDL_SCANCODE_COUNT];
	uint8_t key_prev[SDL_SCANCODE_COUNT];

	uint8_t mouse_down[MOUSE_BUTTON_COUNT];
	uint8_t mouse_prev[MOUSE_BUTTON_COUNT];

	Vec2 mouse_pos;
	Vec2 mouse_prev_pos;

	float mouse_wheel_delta;

	int quit;
} Input_Context;

static Input_Context *input_context(void)
{
	static Input_Context ctx;
	return &ctx;
}

static int key_scancode_of(Key key)
{
	if (key <= KEY_UNKNOWN || key >= KEY_COUNT) {
		return SDL_SCANCODE_UNKNOWN;
	}

	return (int)key_scancode[key];
}

static int mouse_index_of(MouseButton button)
{
	if (button < MOUSE_LEFT || button >= MOUSE_BUTTON_COUNT) {
		return -1;
	}

	return (int)button;
}

int input_key_down(Key key)
{
	Input_Context *ctx = input_context();
	int sc = key_scancode_of(key);

	return ctx->key_down[sc] != 0;
}

int input_key_pressed(Key key)
{
	Input_Context *ctx = input_context();
	int sc = key_scancode_of(key);

	return ctx->key_down[sc] != 0 && ctx->key_prev[sc] == 0;
}

int input_key_released(Key key)
{
	Input_Context *ctx = input_context();
	int sc = key_scancode_of(key);

	return ctx->key_down[sc] == 0 && ctx->key_prev[sc] != 0;
}

int input_mouse_down(MouseButton button)
{
	Input_Context *ctx = input_context();
	int b = mouse_index_of(button);

	return b >= 0 && ctx->mouse_down[b] != 0;
}

int input_mouse_pressed(MouseButton button)
{
	Input_Context *ctx = input_context();
	int b = mouse_index_of(button);

	return b >= 0 && ctx->mouse_down[b] != 0 && ctx->mouse_prev[b] == 0;
}

int input_mouse_released(MouseButton button)
{
	Input_Context *ctx = input_context();
	int b = mouse_index_of(button);

	return b >= 0 && ctx->mouse_down[b] == 0 && ctx->mouse_prev[b] != 0;
}

Vec2 input_mouse_pos(void) { return input_context()->mouse_pos; }

Vec2 input_mouse_delta(void)
{
	Input_Context *ctx = input_context();

	return vec2_sub(ctx->mouse_pos, ctx->mouse_prev_pos);
}

float input_mouse_wheel(void) { return input_context()->mouse_wheel_delta; }

void process_events(void)
{
	Input_Context *ctx = input_context();
	SDL_Event e;

	memcpy(ctx->key_prev, ctx->key_down, sizeof(ctx->key_down));
	memcpy(ctx->mouse_prev, ctx->mouse_down, sizeof(ctx->mouse_down));
	ctx->mouse_prev_pos = ctx->mouse_pos;
	ctx->mouse_wheel_delta = 0.0f;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_EVENT_KEY_DOWN:
			ctx->key_down[e.key.scancode] = 1;
			break;
		case SDL_EVENT_KEY_UP:
			ctx->key_down[e.key.scancode] = 0;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			int b = (int)e.button.button - 1;

			if (b >= 0 && b < MOUSE_BUTTON_COUNT) {
				ctx->mouse_down[b] =
				    (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? 1
									    : 0;
			}
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
			ctx->mouse_pos = vec2(e.motion.x, e.motion.y);
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			ctx->mouse_wheel_delta += e.wheel.y;
			break;
		case SDL_EVENT_QUIT:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			ctx->quit = 1;
			break;
		default:
			break;
		}
	}
}

int window_should_quit(void) { return input_context()->quit; }
