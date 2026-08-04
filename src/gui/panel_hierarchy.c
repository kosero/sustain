#include "gui/panel_hierarchy.h"
#include "core/core.h"
#include "ecs/components.h"
#include "flecs.h"
#include "flecs/private/api_defines.h"
#include "nuklear.h"

enum { NODE_STACK_CAPACITY = 256 };

struct node_task {
	ecs_entity_t entity;
	bool close;
};

static bool node_task_push(struct node_task *stack, int *depth,
			   ecs_entity_t entity, bool close)
{
	if (*depth >= NODE_STACK_CAPACITY) {
		return false;
	}
	stack[(*depth)++] =
	    (struct node_task){.entity = entity, .close = close};
	return true;
}

static const char *node_name(ecs_world_t *world, ecs_entity_t entity)
{
	const char *name = ecs_get_name(world, entity);
	if (name == NULL) {
		name = "Unnamed Entity";
	}
	return name;
}

static int collect_gameobject_children(CoreContext *ctx, ecs_entity_t parent,
				       ecs_entity_t *out, int capacity)
{
	int count = 0;
	ecs_iter_t it = ecs_children(ctx->world, parent);
	while (ecs_children_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			if (count >= capacity) {
				return count;
			}
			if (ecs_has_id(ctx->world, it.entities[i],
				       ecs_id(GameObject))) {
				out[count++] = it.entities[i];
			}
		}
	}
	return count;
}

static void handle_node_selection(CoreContext *ctx, ecs_entity_t entity,
				  nk_bool is_selected, nk_bool was_selected)
{
	if (is_selected && !was_selected) {
		ctx->selected_entity = entity;
	} else if (!is_selected && was_selected) {
		ctx->selected_entity = 0;
	}
}

typedef struct {
	struct nk_style_item sel_normal, sel_hover, sel_pressed;
	struct nk_style_item sel_normal_act, sel_hover_act, sel_pressed_act;
	struct nk_style_item tab_bg;
	struct nk_style_item min_norm, min_hov, min_act;
	struct nk_style_item max_norm, max_hov, max_act;
} TreeStyleBackup;

static TreeStyleBackup push_transparent_tree_style(struct nk_context *nk)
{
	TreeStyleBackup b = {
	    .sel_normal = nk->style.selectable.normal,
	    .sel_hover = nk->style.selectable.hover,
	    .sel_pressed = nk->style.selectable.pressed,
	    .sel_normal_act = nk->style.selectable.normal_active,
	    .sel_hover_act = nk->style.selectable.hover_active,
	    .sel_pressed_act = nk->style.selectable.pressed_active,
	    .tab_bg = nk->style.tab.background,
	    .min_norm = nk->style.tab.node_minimize_button.normal,
	    .min_hov = nk->style.tab.node_minimize_button.hover,
	    .min_act = nk->style.tab.node_minimize_button.active,
	    .max_norm = nk->style.tab.node_maximize_button.normal,
	    .max_hov = nk->style.tab.node_maximize_button.hover,
	    .max_act = nk->style.tab.node_maximize_button.active,
	};

	struct nk_style_item clear = nk_style_item_color(nk_rgba(0, 0, 0, 0));

	nk->style.selectable.normal = clear;
	nk->style.selectable.hover = clear;
	nk->style.selectable.pressed = clear;
	nk->style.selectable.normal_active = clear;
	nk->style.selectable.hover_active = clear;
	nk->style.selectable.pressed_active = clear;

	nk->style.tab.background = clear;
	nk->style.tab.node_minimize_button.normal = clear;
	nk->style.tab.node_minimize_button.hover = clear;
	nk->style.tab.node_minimize_button.active = clear;
	nk->style.tab.node_maximize_button.normal = clear;
	nk->style.tab.node_maximize_button.hover = clear;
	nk->style.tab.node_maximize_button.active = clear;

	return b;
}

static void pop_transparent_tree_style(struct nk_context *nk,
				       const TreeStyleBackup *b)
{
	nk->style.selectable.normal = b->sel_normal;
	nk->style.selectable.hover = b->sel_hover;
	nk->style.selectable.pressed = b->sel_pressed;
	nk->style.selectable.normal_active = b->sel_normal_act;
	nk->style.selectable.hover_active = b->sel_hover_act;
	nk->style.selectable.pressed_active = b->sel_pressed_act;

	nk->style.tab.background = b->tab_bg;
	nk->style.tab.node_minimize_button.normal = b->min_norm;
	nk->style.tab.node_minimize_button.hover = b->min_hov;
	nk->style.tab.node_minimize_button.active = b->min_act;
	nk->style.tab.node_maximize_button.normal = b->max_norm;
	nk->style.tab.node_maximize_button.hover = b->max_hov;
	nk->style.tab.node_maximize_button.active = b->max_act;
}

static void draw_entity_node(CoreContext *ctx, ecs_entity_t root)
{
	struct node_task stack[NODE_STACK_CAPACITY];
	int depth = 0;
	if (!node_task_push(stack, &depth, root, false)) {
		return;
	}

	struct nk_context *nk = ctx->nk_ctx;

	TreeStyleBackup style_backup = push_transparent_tree_style(nk);

	while (depth > 0) {
		struct node_task task = stack[--depth];

		if (task.close) {
			nk_tree_element_pop(nk);
			continue;
		}

		nk_bool is_selected = (ctx->selected_entity == task.entity);
		nk_bool was_selected = is_selected;

		struct nk_rect row_bounds = nk_widget_bounds(nk);

		if (is_selected) {
			struct nk_command_buffer *canvas =
			    nk_window_get_canvas(nk);
			nk_fill_rect(canvas, row_bounds, 2.0f,
				     nk_rgba(65, 105, 225, 120));
		}

		if (nk_tree_element_push_id(
			nk, NK_TREE_NODE, node_name(ctx->world, task.entity),
			NK_MINIMIZED, &is_selected, (int)task.entity)) {
			ecs_entity_t children[NODE_STACK_CAPACITY];
			int child_count = collect_gameobject_children(
			    ctx, task.entity, children, NODE_STACK_CAPACITY);

			if (depth + 1 + child_count < NODE_STACK_CAPACITY) {
				node_task_push(stack, &depth, 0, true);
				for (int i = child_count - 1; i >= 0; i--) {
					node_task_push(stack, &depth,
						       children[i], false);
				}
			}
		}

		handle_node_selection(ctx, task.entity, is_selected,
				      was_selected);
	}

	pop_transparent_tree_style(nk, &style_backup);
}

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
