#include "gui/gui.h"
#include "core/config.h"
#include "gui/panel_hierarchy.h"
#include "gui/panel_properties.h"
#include "gui/panel_viewport.h"
#include "microui.h"

static const int gui_panel_width = 350;

static void force_root_window_rect(mu_Context *mu, int width, int height)
{
	mu_Container *root = mu_get_container(mu, "EditorRoot");
	if (root != NULL) {
		root->rect = mu_rect(0, 0, width, height);
	}
}

void gui_render_ui(CoreContext *ctx)
{
	mu_Context *mu = ctx->mu_ctx;
	WindowProperty *w = get_window_property();
	int screen_w = w->width;
	int screen_h = w->height;

	mu_begin(mu);
	force_root_window_rect(mu, screen_w, screen_h);

	if (mu_begin_window_ex(
		mu, "EditorRoot", mu_rect(0, 0, screen_w, screen_h),
		MU_OPT_NOFRAME | MU_OPT_NOTITLE | MU_OPT_NORESIZE |
		    MU_OPT_NOCLOSE | MU_OPT_NOSCROLL)) {
		mu_Container *root = mu_get_current_container(mu);
		int spacing = mu->style->spacing;
		int middle_w = mu_max(
		    root->body.w - (gui_panel_width * 2) - (spacing * 2), 1);
		int widths[3] = {gui_panel_width, middle_w, gui_panel_width};
		mu_layout_row(mu, 3, widths, -1);

		gui_panel_hierarchy(ctx);
		gui_panel_viewport(ctx);
		gui_panel_properties(ctx);
	}
	mu_end_window(mu);

	mu_end(mu);
}
