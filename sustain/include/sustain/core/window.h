#ifndef WINDOW_H
#define WINDOW_H

void window_init(const int width, const int height, const char *title,
                 const int fullscreen);
void window_close(void);

#endif // !WINDOW_H
