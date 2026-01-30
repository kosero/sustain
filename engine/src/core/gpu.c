#include "sustain/core/gpu.h"

SDL_GPUDevice *SN_Init_Graphics_Device(SDL_Window *window) {
  SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                                  SDL_GPU_SHADERFORMAT_DXIL |
                                                  SDL_GPU_SHADERFORMAT_METALLIB,
                                              true, NULL);
  if (device) {
    SDL_ClaimWindowForGPUDevice(device, window);
  }
  return device;
}

void SN_Destroy_Graphics_Device(SDL_GPUDevice *gpu_device, SDL_Window *window) {
  if (gpu_device) {
    SDL_WaitForGPUIdle(gpu_device);
    if (window) {
      SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
    }
    SDL_DestroyGPUDevice(gpu_device);
  }
}
