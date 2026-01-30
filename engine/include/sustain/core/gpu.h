#ifndef SUSTAIN_CORE_GPU_H
#define SUSTAIN_CORE_GPU_H

#include <SDL3/SDL.h>

bool SN_Init_Graphics_Device(void);
void SN_Destroy_Graphics_Device(void);
void SN_Set_GPU_Window(SDL_Window *window);
SDL_GPUDevice *SN_Get_GPU_Device(void);

#endif
