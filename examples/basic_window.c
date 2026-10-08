#include "sustain.h"

int main(void)
{
	init_window(800, 600, "basic window");

	while (!window_quit()) {
		frame_clear((Color){48, 48, 48, 255});

		next_frame();
	}

	close_window();
	return 0;
}
