#include "sustain.h"
#include <assert.h>

int main(void)
{
	assert(init_window(800, 600, "basic window") == 0);

	while (!window_quit()) {
		process_events();

		if (input_key_pressed(KEY_ESCAPE)) {
			break;
		}

		frame_clear((Color){48, 48, 48, 255});

		next_frame();
	}

	close_window();
	return 0;
}
