#include "gui/panel_hierarchy.h"
#include "raylib-nuklear.h"

void gui_panel_hierarchy(CoreContext *ctx) {
  if (nk_group_begin(ctx->nk_ctx, "Hierarchy",
                     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    nk_layout_row_dynamic(ctx->nk_ctx, 20, 1);

    ecs_iter_t it = ecs_query_iter(ctx->world, ctx->hierarchy_query);
    while (ecs_query_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t e = it.entities[i];
        const char *name = ecs_get_name(ctx->world, e);
        if (!name) {
          name = "Unnamed Entity";
        }

        nk_bool is_selected = (ctx->selected_entity == e);
        if (nk_selectable_label(ctx->nk_ctx, name, NK_TEXT_LEFT, &is_selected)) {
          if (is_selected) {
            ctx->selected_entity = e;
          } else {
            ctx->selected_entity = 0;
          }
        }
      }
    }

    nk_group_end(ctx->nk_ctx);
  }
}
