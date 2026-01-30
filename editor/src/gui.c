#include "gui.h"

void SN_Gui_Init(SDL_Window *window, SDL_GPUDevice *gpu_device) {
  igCreateContext(NULL);
  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui_ImplSDL3_InitForSDLGPU(window);

  ImGui_ImplSDLGPU3_InitInfo init_info = {0};
  init_info.Device = gpu_device;
  init_info.ColorTargetFormat =
      SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
  init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
  ImGui_ImplSDLGPU3_Init(&init_info);
}

void SN_Gui_NewFrame() {
  ImGui_ImplSDLGPU3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  igNewFrame();
}

void SN_Gui_Render(SDL_Window *window, SDL_GPUDevice *gpu_device) {
  igRender();

  SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(gpu_device);
  SDL_GPUTexture *swapchain_tex;

  if (SDL_AcquireGPUSwapchainTexture(cmd_buf, window, &swapchain_tex, NULL,
                                     NULL)) {
    if (swapchain_tex != NULL) {
      ImGui_ImplSDLGPU3_PrepareDrawData(igGetDrawData(), cmd_buf);

      SDL_GPUColorTargetInfo target_info = {0};
      target_info.texture = swapchain_tex;
      target_info.clear_color = (SDL_FColor){0.11f, 0.11f, 0.11f, 1.0f};
      target_info.load_op = SDL_GPU_LOADOP_CLEAR;
      target_info.store_op = SDL_GPU_STOREOP_STORE;

      SDL_GPURenderPass *pass =
          SDL_BeginGPURenderPass(cmd_buf, &target_info, 1, NULL);
      ImGui_ImplSDLGPU3_RenderDrawData(igGetDrawData(), cmd_buf, pass, NULL);
      SDL_EndGPURenderPass(pass);
    }
  }

  SDL_SubmitGPUCommandBuffer(cmd_buf);
}

void SN_Quit_Gui(void) {
  ImGui_ImplSDLGPU3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  igDestroyContext(NULL);
}
