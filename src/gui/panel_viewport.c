#include "gui/panel_viewport.h"
#include "core/input.h"
#include "math/math.h"
#include "microui.h"
#include "renderer/renderer.h"

void gui_panel_viewport(CoreContext *ctx)
{
	mu_Context *mu = ctx->mu_ctx;
	mu_begin_panel_ex(mu, "Viewport", MU_OPT_NOSCROLL);

	int width = -1;
	mu_layout_row(mu, 1, &width, -1);
	mu_Rect r = mu_layout_next(mu);

	if (r.w > 0 && r.h > 0) {
		bool is_hovered =
		    rect_contains(ctx->renderer.viewport_draw_rect,
				  input_mouse_x(), input_mouse_y());
		editor_camera_update(&ctx->editor_camera, is_hovered);

		renderer_viewport_resize(&ctx->renderer, r.w, r.h);
		renderer_draw_viewport(&ctx->renderer,
				       &ctx->editor_camera.camera, ctx->world);

		ctx->renderer.viewport_draw_rect =
		    (Rect){(float)r.x, (float)r.y, (float)r.w, (float)r.h};
	}

	mu_end_panel(mu);
}
