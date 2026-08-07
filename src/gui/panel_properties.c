#include "gui/panel_properties.h"
#include "SDL3/SDL_timer.h"
#include "ecs/components.h"
#include "microui.h"

#include <stdbool.h>
#include <stdio.h>

enum { DOUBLE_CLICK_WINDOW_MS = 400 };
enum { CLICK_TRACK_CAPACITY = 16 };

struct click_track {
	mu_Id id;
	Uint64 time;
};

static struct click_track *get_click_track_table(void)
{
	static struct click_track entries[CLICK_TRACK_CAPACITY];
	return entries;
}

static int *get_click_track_count(void)
{
	static int count = 0;
	return &count;
}

static bool track_double_click(mu_Id id, Uint64 now)
{
	struct click_track *entries = get_click_track_table();
	int *count = get_click_track_count();
	for (int i = 0; i < *count; i++) {
		if (entries[i].id == id) {
			bool is_double = (now - entries[i].time) <
					 (Uint64)DOUBLE_CLICK_WINDOW_MS;
			entries[i].time = now;
			return is_double;
		}
	}
	if (*count < CLICK_TRACK_CAPACITY) {
		entries[*count].id = id;
		entries[*count].time = now;
		(*count)++;
	}
	return false;
}

static void enter_number_edit(mu_Context *mu, mu_Id id, float value)
{
	mu->number_edit = id;
	(void)sprintf(mu->number_edit_buf, MU_REAL_FMT, value);
}

static void draw_vec3_property(mu_Context *mu, const char *label, vec3s *vec,
			       float min, float max, float step)
{
	int width = -1;
	mu_layout_row(mu, 1, &width, 0);
	mu_label(mu, label);

	mu_Container *panel = mu_get_current_container(mu);
	int padding = mu->style->padding;
	int spacing = mu->style->spacing;
	int body_w = panel->body.w - (padding * 2);
	int label_w = 20;
	int field_w = mu_max((body_w - (label_w * 3) - (spacing * 5)) / 3, 1);
	int widths[6] = {label_w, field_w, label_w, field_w, label_w, field_w};
	const char *axis[3] = {"X", "Y", "Z"};
	mu_layout_row(mu, 6, widths, 0);
	for (int i = 0; i < 3; i++) {
		mu_draw_control_text(mu, axis[i], mu_layout_next(mu),
				     MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
		float *field = &vec->raw[i];
		if (mu_number(mu, field, step) & MU_RES_CHANGE) {
			*field = mu_clamp(*field, min, max);
		}
		mu_Id id =
		    mu_get_id(mu, (const void *)&field, (int)sizeof(field));
		if (mu->mouse_pressed == MU_MOUSE_LEFT && mu->focus == id &&
		    track_double_click(id, SDL_GetTicks())) {
			enter_number_edit(mu, id, *field);
		}
	}
}

static void draw_transform_section(mu_Context *mu, Transform3D *t)
{
	if (mu_header_ex(mu, "Transform3D", MU_OPT_EXPANDED)) {
		draw_vec3_property(mu, "Position", &t->position, -1000.0f,
				   1000.0f, 0.1f);
		draw_vec3_property(mu, "Rotation", &t->rotation, -360.0f,
				   360.0f, 1.0f);
		draw_vec3_property(mu, "Scale", &t->scale, -100.0f, 100.0f,
				   0.1f);
	}
}

static void draw_color_slider(mu_Context *mu, const char *label, float *value)
{
	char buf[16];
	mu_label(mu, label);

	float last = *value;
	float v = last;
	mu_Id id = mu_get_id(mu, (const void *)&value, (int)sizeof(value));
	mu_Rect base = mu_layout_next(mu);

	mu_update_control(mu, id, base, 0);

	if (mu->focus == id &&
	    (mu->mouse_down | mu->mouse_pressed) == MU_MOUSE_LEFT) {
		int x = mu_clamp(mu->mouse_pos.x - base.x, 0, base.w - 1);
		int span = mu_max(base.w - 1, 1);
		v = 255.0f * (float)x / (float)span;
	}

	*value = v;

	mu_draw_control_frame(mu, id, base, MU_COLOR_BASE, 0);

	int thumb_w = mu->style->thumb_size;
	int thumb_x =
	    mu_max((int)((v / 255.0f) * (float)(base.w - thumb_w)), 0);
	mu_Rect thumb = mu_rect(base.x + thumb_x, base.y, thumb_w, base.h);
	mu_draw_control_frame(mu, id, thumb, MU_COLOR_BUTTON, 0);

	(void)snprintf(buf, sizeof(buf), "%.0f", v);
	mu_draw_control_text(mu, buf, base, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
}

static void draw_mesh_renderer_section(mu_Context *mu, MeshRenderer *mr)
{
	if (mu_header_ex(mu, "MeshRenderer", MU_OPT_EXPANDED)) {
		const char *names[] = {"Cube", "Sphere"};

		mu_layout_row(mu, 1, (int[]){-1}, 0);
		mu_label(mu, "Type");

		mu_layout_row(mu, 1, (int[]){-1}, 0);
		if (mu_button(mu, names[mr->type])) {
			mu_open_popup(mu, "MeshTypePopup");
		}
		if (mu_begin_popup(mu, "MeshTypePopup")) {
			mu_layout_row(mu, 1, (int[]){120}, 0);
			if (mu_button(mu, "Cube")) {
				mr->type = PRIMITIVE_CUBE;
			}
			if (mu_button(mu, "Sphere")) {
				mr->type = PRIMITIVE_SPHERE;
			}
			mu_end_popup(mu);
		}

		mu_layout_row(mu, 1, (int[]){-1}, 0);
		mu_label(mu, "Color");

		float rgba[4] = {
		    (float)mr->color.r,
		    (float)mr->color.g,
		    (float)mr->color.b,
		    (float)mr->color.a,
		};

		mu_layout_row(mu, 2, (int[]){28, -1}, 0);
		draw_color_slider(mu, "R", &rgba[0]);
		draw_color_slider(mu, "G", &rgba[1]);
		draw_color_slider(mu, "B", &rgba[2]);
		draw_color_slider(mu, "A", &rgba[3]);

		mu_layout_row(mu, 1, (int[]){-1}, 24);
		mu_draw_rect(mu, mu_layout_next(mu),
			     mu_color((int)rgba[0], (int)rgba[1], (int)rgba[2],
				      (int)rgba[3]));

		mr->color =
		    (Color){(unsigned char)rgba[0], (unsigned char)rgba[1],
			    (unsigned char)rgba[2], (unsigned char)rgba[3]};
	}
}

void gui_panel_properties(CoreContext *ctx)
{
	mu_Context *mu = ctx->mu_ctx;
	mu_begin_panel_ex(mu, "Properties", 0);

	int width = -1;

	if (ctx->selected_entity == 0) {
		mu_layout_row(mu, 1, &width, 0);
		mu_label(mu, "No entity selected");
		mu_end_panel(mu);
		return;
	}

	const char *name = ecs_get_name(ctx->world, ctx->selected_entity);
	if (!name) {
		name = "Unnamed Entity";
	}
	mu_layout_row(mu, 1, &width, 0);
	mu_draw_control_text(mu, name, mu_layout_next(mu), MU_COLOR_TEXT,
			     MU_OPT_ALIGNCENTER);

	Transform3D *transform =
	    ecs_get_mut(ctx->world, ctx->selected_entity, Transform3D);
	if (transform) {
		draw_transform_section(mu, transform);
	}

	MeshRenderer *mesh =
	    ecs_get_mut(ctx->world, ctx->selected_entity, MeshRenderer);
	if (mesh) {
		draw_mesh_renderer_section(mu, mesh);
	}

	mu_end_panel(mu);
}
