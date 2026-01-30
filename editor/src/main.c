#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "gui.h"
#include "sustain/core/gpu.h"
#include "sustain/core/window.h"

int main(int argc, char *argv[]) {
  if (SN_Init_Window(800, 600, "Sustain Editor", true) != 0) {
    return 1;
  }

  SDL_Window *window = SN_Get_Window();

  SDL_GPUDevice *gpu_device = SN_Init_Graphics_Device(window);
  SN_Gui_Init(window, gpu_device);

  SDL_Event event;
  bool running = true;
  ImGuiIO *io = igGetIO();

  while (running) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    SN_Gui_NewFrame();
    igBegin("SustainEditor", NULL, 0);
    igText("FPS: %.1f", io->Framerate);
    igEnd();
    SN_Gui_Render(window, gpu_device);
  }

  SN_Quit_Gui();
  SN_Destroy_Graphics_Device(gpu_device, window);
  SN_Window_Close();
  return 0;
}
