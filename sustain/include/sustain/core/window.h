#ifndef SUSTAIN_GRAPHICS_WINDOW_H
#define SUSTAIN_GRAPHICS_WINDOW_H

#if defined(WIN32) && !defined(_DEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

void window_init(const int width, const int height, const char *title,
                 const int fullscreen);
void window_close(void);

#endif // !WINDOW_H
