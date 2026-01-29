#ifndef SUSTAIN_CORE_WINDOW_H
#define SUSTAIN_CORE_WINDOW_H

#include <SDL3/SDL.h>

int SN_Init_Window(const int width, const int height, const char *title,
                   bool vsync);
void SN_Window_Close(void);
void SN_Clear_Background(void);

SDL_Renderer *SN_Get_Renderer();
SDL_Window *SN_Get_Window();

#endif
