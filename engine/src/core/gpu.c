#include "sustain/core/gpu.h"
#include "sustain/core/log.h"
#include "sustain/core/window.h"
#include <SDL3/SDL.h>

static SDL_GPUDevice *device;

bool SN_Init_Graphics_Device(void) {
  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                   SDL_GPU_SHADERFORMAT_DXIL |
                                   SDL_GPU_SHADERFORMAT_METALLIB,
                               true, NULL);
  if (device) {
    SDL_ClaimWindowForGPUDevice(device, SN_Get_Window());

    SDL_PropertiesID props = SDL_GetGPUDeviceProperties(device);
    const char *gpu_name = SDL_GetStringProperty(
        props, SDL_PROP_GPU_DEVICE_NAME_STRING, "unknown");
    const char *driver_name = SDL_GetStringProperty(
        props, SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING, "unknown");
    const char *driver_info = SDL_GetStringProperty(
        props, SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING, "unknown");

    SLOG_INFO("[GPU]: %s", gpu_name);
    SLOG_INFO("[Driver And Version]: %s %s", driver_name, driver_info);
  } else {
    SLOG_ERROR("Failed to initialize GPU device.\n");
    return false;
  }
  return true;
}

void SN_Destroy_Graphics_Device(void) {
  if (!device) {
    SLOG_WARN("SN_Set_GPU_Window: device == NULL");
    return;
  }

  if (device) {
    SDL_WaitForGPUIdle(device);
    if (SN_Get_Window()) {
      SDL_ReleaseWindowFromGPUDevice(device, SN_Get_Window());
    }
    SDL_DestroyGPUDevice(device);
  }
}

void SN_Set_GPU_Window(SDL_Window *window) {
  if (!device) {
    SLOG_ERROR("SN_Set_GPU_Window: device == NULL");
    return;
  }
  if (!window) {
    SLOG_ERROR("SN_Set_GPU_Window: window == NULL");
    return;
  }

  bool ok = SDL_ClaimWindowForGPUDevice(device, window);
  if (ok) {
    const char *driver = SDL_GetGPUDeviceDriver(device);
    SLOG_ERROR("GPU window claim OK. Backend: %s", driver);
  } else {
    SLOG_ERROR("GPU window claim FAILED: %s", SDL_GetError());
  }
}

SDL_GPUDevice *SN_Get_GPU_Device(void) {
  if (!device) {
    SLOG_ERROR("SN_Set_GPU_Window: device == NULL");
    return NULL;
  }

  return device;
}
