#include "gui/gui.h"
#include "core/config.h"
#include "gui/panel_hierarchy.h"
#include "gui/panel_properties.h"
#include "gui/panel_viewport.h"
#include "nuklear.h"

static const float gui_panel_width = 350.0f;

void gui_render_ui(CoreContext *ctx)
{
	WindowProperty *w = get_window_property();
	float screen_w = (float)w->width;
	float screen_h = (float)w->height;

	if (nk_begin(ctx->nk_ctx, "EditorRoot",
		     nk_rect(0, 0, screen_w, screen_h),
		     NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
		struct nk_rect content =
		    nk_window_get_content_region(ctx->nk_ctx);
		float row_h = content.h;

		nk_layout_row_begin(ctx->nk_ctx, NK_STATIC, row_h, 3);

		// left panel
		nk_layout_row_push(ctx->nk_ctx, gui_panel_width);
		gui_panel_hierarchy(ctx);

		// center viewport
		float viewport_w = content.w - (gui_panel_width * 2.0f);
		if (viewport_w < 1.0f) {
			viewport_w = 1.0f;
		}
		nk_layout_row_push(ctx->nk_ctx, viewport_w);
		gui_panel_viewport(ctx);

		// right panel
		nk_layout_row_push(ctx->nk_ctx, gui_panel_width);
		gui_panel_properties(ctx);

		nk_layout_row_end(ctx->nk_ctx);
	}

	nk_end(ctx->nk_ctx);
}
