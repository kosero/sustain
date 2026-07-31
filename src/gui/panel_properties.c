#include "gui/panel_properties.h"
#include "raylib-nuklear.h"

void gui_panel_properties(CoreContext *ctx) {
  if (nk_group_begin(ctx->nk_ctx, "Properties",
                     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    nk_layout_row_dynamic(ctx->nk_ctx, 20, 1);
    nk_group_end(ctx->nk_ctx);
  }
}
