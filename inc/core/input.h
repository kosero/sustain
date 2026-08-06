#ifndef SUSTAIN_INPUT_H
#define SUSTAIN_INPUT_H

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"
#include <stdbool.h>

void input_frame_begin(void);
void input_handle_event(const SDL_Event *evt);

bool input_key_down(SDL_Scancode scancode);
bool input_key_pressed(SDL_Scancode scancode);

bool input_mouse_button_pressed(int button);
bool input_mouse_button_released(int button);

float input_mouse_x(void);
float input_mouse_y(void);
float input_mouse_delta_x(void);
float input_mouse_delta_y(void);

bool input_window_resized(void);
int input_window_width(void);
int input_window_height(void);
bool input_quit_requested(void);

void input_set_cursor_grabbed(bool grabbed);

#endif // SUSTAIN_INPUT_H
