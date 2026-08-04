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

static void draw_entity_node(CoreContext *ctx, ecs_entity_t root)
{
	struct node_task stack[NODE_STACK_CAPACITY];
	int depth = 0;
	if (!node_task_push(stack, &depth, root, false)) {
		return;
	}

	while (depth > 0) {
		struct node_task task = stack[--depth];

		if (task.close) {
			nk_tree_element_pop(ctx->nk_ctx);
			continue;
		}

		nk_bool is_selected = (ctx->selected_entity == task.entity);
		nk_bool was_selected = is_selected;

		if (nk_tree_element_push_id(ctx->nk_ctx, NK_TREE_NODE,
					    node_name(ctx->world, task.entity),
					    NK_MINIMIZED, &is_selected,
					    (int)task.entity)) {
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
