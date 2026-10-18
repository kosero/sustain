#include "sustain.h"
#include <assert.h>

int main(void)
{
	assert(init_window(800, 600, "draw pixels") == 0);

	while (!window_should_quit()) {
		process_events();

		if (input_key_pressed(KEY_ESCAPE)) {
			break;
		}

		frame_clear((Color){24, 24, 24, 255});

		for (int i = -20; i <= 20; i++) {
			draw_pixel(400 + i, 300, (Color){255, 200, 0, 255});
			draw_pixel(400, 300 + i, (Color){255, 200, 0, 255});
		}

		next_frame();
	}

	close_window();
	return 0;
}
