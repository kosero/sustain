#include "sustain/renderer/renderer.h"
#include "sustain/core/window.h"

void SN_Clear_Background(SN_Color color) {
  SDL_SetRenderDrawColor(SN_Get_Renderer(), color.r, color.g, color.g, color.a);
  SDL_RenderClear(SN_Get_Renderer());
}
