#ifndef SUSTAIN_CORE_GPU_H
#define SUSTAIN_CORE_GPU_H

#include <SDL3/SDL.h>

SDL_GPUDevice *SN_Init_Graphics_Device(SDL_Window *window);
void SN_Destroy_Graphics_Device(SDL_GPUDevice *gpu_device, SDL_Window *window);

#endif
