#include "gui/panel_viewport.h"
#include "core/input.h"
#include "math/math.h"
#include "nuklear.h"
#include "renderer/renderer.h"

void gui_panel_viewport(CoreContext *ctx)
{
	if (nk_group_begin(ctx->nk_ctx, "ViewportGroup",
			   NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
		struct nk_rect vp_content =
		    nk_window_get_content_region(ctx->nk_ctx);
		int vp_w = (int)vp_content.w;
		int vp_h = (int)vp_content.h;

		if (vp_w > 0 && vp_h > 0) {
			bool is_hovered =
			    rect_contains(ctx->renderer.viewport_draw_rect,
					  input_mouse_x(), input_mouse_y());
			editor_camera_update(&ctx->editor_camera, is_hovered);

			renderer_viewport_resize(&ctx->renderer, vp_w, vp_h);
			renderer_draw_viewport(&ctx->renderer,
					       &ctx->editor_camera.camera,
					       ctx->world);

			nk_layout_row_static(ctx->nk_ctx, (float)vp_h, vp_w, 1);
			struct nk_rect bounds = nk_widget_bounds(ctx->nk_ctx);
			ctx->renderer.viewport_draw_rect =
			    (Rect){bounds.x, bounds.y, bounds.w, bounds.h};
			nk_spacing(ctx->nk_ctx, 1);
		}
		nk_group_end(ctx->nk_ctx);
	}
}
