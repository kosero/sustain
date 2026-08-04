#include "core/config.h"
#include "SDL3/SDL.h"

static WindowProperty **get_window_property_ptr_internal(void)
{
	static WindowProperty *window = NULL;
	return &window;
}

WindowProperty *get_window_property(void)
{
	WindowProperty **window_ptr = get_window_property_ptr_internal();
	if (*window_ptr == NULL) {
		static WindowProperty default_window = {0};
		*window_ptr = &default_window;
	}
	return *window_ptr;
}

void set_window_property(WindowProperty *window)
{
	*get_window_property_ptr_internal() = window;
}

void window_property_init(void)
{
	WindowProperty *window = get_window_property();

	window->title = "Sustain";
	window->width = 800;
	window->height = 600;
	window->flags = (unsigned int)SDL_WINDOW_RESIZABLE;
}
