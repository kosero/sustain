#include "gui/panel_properties.h"
#include "ecs/components.h"
#include "nuklear.h"

static void draw_vec3_property(struct nk_context *nk, const char *label,
			       vec3s *vec, float min, float max, float step)
{
	struct nk_color colors[3] = {
	    nk_rgb(220, 80, 80),  /* X - red */
	    nk_rgb(80, 200, 80),  /* Y - green */
	    nk_rgb(80, 140, 220), /* Z - blue */
	};

	nk_layout_row_dynamic(nk, 18, 1);
	nk_label(nk, label, NK_TEXT_LEFT);

	nk_layout_row_dynamic(nk, 22, 3);

	for (int i = 0; i < 3; i++) {
		struct nk_color old = nk->style.property.border_color;
		nk->style.property.label_normal = colors[i];

		char id[4] = {'#', (char)('x' + i), '\0'};
		nk_property_float(nk, id, min, &vec->raw[i], max, step,
				  step * 0.5f);

		nk->style.property.label_normal = old;
	}
}
static void draw_transform_section(struct nk_context *nk, Transform3D *t)
{
	if (nk_tree_push(nk, NK_TREE_TAB, "Transform3D", NK_MAXIMIZED)) {
		draw_vec3_property(nk, "Position", &t->position, -1000.0f,
				   1000.0f, 0.1f);
		draw_vec3_property(nk, "Rotation", &t->rotation, -360.0f,
				   360.0f, 1.0f);
		draw_vec3_property(nk, "Scale", &t->scale, -100.0f, 100.0f,
				   0.1f);
		nk_tree_pop(nk);
	}
}

static void draw_mesh_renderer_section(struct nk_context *nk, MeshRenderer *mr)
{
	if (nk_tree_push(nk, NK_TREE_TAB, "MeshRenderer", NK_MAXIMIZED)) {
		// primitive type
		nk_layout_row_dynamic(nk, 20, 1);
		nk_label(nk, "Type", NK_TEXT_LEFT);
		nk_layout_row_dynamic(nk, 25, 1);
		const char *types[] = {"Cube", "Sphere"};
		int current = (int)mr->type;
		current = nk_combo(nk, types, 2, current, 25,
				   nk_vec2(nk_widget_width(nk), 100));
		mr->type = (PrimitiveType)current;

		// color
		nk_layout_row_dynamic(nk, 20, 1);
		nk_label(nk, "Color", NK_TEXT_LEFT);

		struct nk_colorf col = {
		    (float)mr->color.r / 255.0f, (float)mr->color.g / 255.0f,
		    (float)mr->color.b / 255.0f, (float)mr->color.a / 255.0f};

		nk_layout_row_dynamic(nk, 120, 1);
		col = nk_color_picker(nk, col, NK_RGBA);

		mr->color = (Color){(unsigned char)(col.r * 255.0f),
				    (unsigned char)(col.g * 255.0f),
				    (unsigned char)(col.b * 255.0f),
				    (unsigned char)(col.a * 255.0f)};

		nk_tree_pop(nk);
	}
}

void gui_panel_properties(CoreContext *ctx)
{
	if (nk_group_begin(ctx->nk_ctx, "Properties",
			   NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
		if (ctx->selected_entity == 0) {
			nk_layout_row_dynamic(ctx->nk_ctx, 20, 1);
			nk_label(ctx->nk_ctx, "No entity selected",
				 NK_TEXT_CENTERED);
			nk_group_end(ctx->nk_ctx);
			return;
		}

		// entity name header
		const char *name =
		    ecs_get_name(ctx->world, ctx->selected_entity);
		if (!name) {
			name = "Unnamed Entity";
		}
		nk_layout_row_dynamic(ctx->nk_ctx, 25, 1);
		nk_label(ctx->nk_ctx, name, NK_TEXT_CENTERED);

		// transform3d section
		Transform3D *transform =
		    ecs_get_mut(ctx->world, ctx->selected_entity, Transform3D);
		if (transform) {
			draw_transform_section(ctx->nk_ctx, transform);
		}

		// mesh renderer section
		MeshRenderer *mesh =
		    ecs_get_mut(ctx->world, ctx->selected_entity, MeshRenderer);
		if (mesh) {
			draw_mesh_renderer_section(ctx->nk_ctx, mesh);
		}

		nk_group_end(ctx->nk_ctx);
	}
}
