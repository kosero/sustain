#ifndef SUSTAIN_H
#define SUSTAIN_H

#include <stdint.h>

typedef struct Color {
	uint8_t r, g, b, a;
} Color;

int init_window(uint32_t width, uint32_t height, const char *title);
int window_quit(void);
void close_window(void);

void frame_clear(Color color);
void next_frame(void);

#endif // SUSTAIN_H
