#include "gui/panel_hierarchy.h"
#include "components.h"
#include "flecs.h"
#include "raylib-nuklear.h"

// NOLINTBEGIN(misc-no-recursion)
static void draw_entity_node(CoreContext *ctx, ecs_entity_t e)
{
	const char *name = ecs_get_name(ctx->world, e);
	if (!name) {
		name = "Unnamed Entity";
	}

	nk_bool is_selected = (ctx->selected_entity == e);
	nk_bool was_selected = is_selected;

	if (nk_tree_element_push_id(ctx->nk_ctx, NK_TREE_NODE, name,
				    NK_MINIMIZED, &is_selected, (int)e)) {
		ecs_iter_t children = ecs_children(ctx->world, e);
		while (ecs_children_next(&children)) {
			for (int i = 0; i < children.count; i++) {
				if (ecs_has_id(ctx->world, children.entities[i],
					       ecs_id(GameObject))) {
					draw_entity_node(ctx,
							 children.entities[i]);
				}
			}
		}
		nk_tree_element_pop(ctx->nk_ctx);
	}

	if (is_selected && !was_selected) {
		ctx->selected_entity = e;
	} else if (!is_selected && was_selected) {
		ctx->selected_entity = 0;
	}
}
// NOLINTEND(misc-no-recursion)

void gui_panel_hierarchy(CoreContext *ctx)
{
	if (nk_group_begin(ctx->nk_ctx, "Hierarchy",
			   NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
		nk_layout_row_dynamic(ctx->nk_ctx, 1, 1);

		if (nk_tree_push(ctx->nk_ctx, NK_TREE_TAB, "RootScene",
				 NK_MAXIMIZED)) {
			ecs_iter_t it =
			    ecs_query_iter(ctx->world, ctx->hierarchy_query);
			while (ecs_query_next(&it)) {
				for (int i = 0; i < it.count; i++) {
					draw_entity_node(ctx, it.entities[i]);
				}
			}
			nk_tree_pop(ctx->nk_ctx);
		}

		nk_group_end(ctx->nk_ctx);
	}
}
