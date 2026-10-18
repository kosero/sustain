#ifndef SUSTAIN_H
#define SUSTAIN_H

#include <stdint.h>

typedef struct Color {
	uint8_t r, g, b, a;
} Color;

typedef struct Vec2 {
	float x, y;
} Vec2;

typedef struct Mat4 {
	float m[16];
} Mat4;

Vec2 vec2(float x, float y);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_scale(Vec2 v, float s);

int init_window(uint32_t width, uint32_t height, const char *title);
int window_quit(void);
void close_window(void);

void process_events(void);

void frame_clear(Color color);
void next_frame(void);

typedef enum Key {
	KEY_UNKNOWN = 0,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_ZERO,
	KEY_ONE,
	KEY_TWO,
	KEY_THREE,
	KEY_FOUR,
	KEY_FIVE,
	KEY_SIX,
	KEY_SEVEN,
	KEY_EIGHT,
	KEY_NINE,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_SPACE,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_TAB,
	KEY_BACKSPACE,
	KEY_DELETE,
	KEY_INSERT,
	KEY_HOME,
	KEY_END,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,

	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,

	KEY_LEFT_SHIFT,
	KEY_RIGHT_SHIFT,
	KEY_LEFT_CTRL,
	KEY_RIGHT_CTRL,
	KEY_LEFT_ALT,
	KEY_RIGHT_ALT,
	KEY_LEFT_SUPER,
	KEY_RIGHT_SUPER,

	KEY_MINUS,
	KEY_EQUALS,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_LEFT_BRACKET,
	KEY_RIGHT_BRACKET,
	KEY_BACKSLASH,
	KEY_GRAVE,

	KEY_COUNT,
} Key;

typedef enum MouseButton {
	MOUSE_LEFT = 0,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_X1,
	MOUSE_X2,
	MOUSE_BUTTON_COUNT,
} MouseButton;

int input_key_down(Key key);
int input_key_pressed(Key key);
int input_key_released(Key key);

int input_mouse_down(MouseButton button);
int input_mouse_pressed(MouseButton button);
int input_mouse_released(MouseButton button);

Vec2 input_mouse_pos(void);
Vec2 input_mouse_delta(void);
float input_mouse_wheel(void);

#endif // SUSTAIN_H
