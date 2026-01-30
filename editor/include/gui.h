#ifndef EDITOR_GUI_H
#define EDITOR_GUI_H

#include <SDL3/SDL.h>

#include "cimgui.h"
#include "cimgui_impl.h"

#define igGetIO igGetIO_Nil

void SN_Gui_Init(SDL_Window *window, SDL_GPUDevice *gpu_device);
void SN_Gui_NewFrame();
void SN_Gui_Render(SDL_Window *window, SDL_GPUDevice *gpu_device);
void SN_Quit_Gui(void);

#endif
