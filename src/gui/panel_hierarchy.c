#include "gui/panel_hierarchy.h"
#include "core/core.h"
#include "core/log.h"
#include "ecs/components.h"
#include "flecs.h"
#include "flecs/private/api_defines.h"
#include "microui.h"

enum { NODE_STACK_CAPACITY = 256 };
enum { TREE_STATE_CAPACITY = 256 };

enum {
	TREE_ROW_ICON_CLICKED = (1 << 0),
	TREE_ROW_CLICKED = (1 << 1),
};

struct node_task {
	ecs_entity_t entity;
	int level;
	bool close;
};

static bool node_task_push(struct node_task *stack, int *depth,
			   ecs_entity_t entity, int level, bool close)
{
	if (*depth >= NODE_STACK_CAPACITY) {
		return false;
	}
	stack[(*depth)++] = (struct node_task){
	    .entity = entity, .level = level, .close = close};
	return true;
}

struct tree_state_entry {
	ecs_entity_t entity;
	bool expanded;
};

static struct tree_state_entry *get_tree_state_table(void)
{
	static struct tree_state_entry states[TREE_STATE_CAPACITY];
	return states;
}

static int *get_tree_state_count(void)
{
	static int count = 0;
	return &count;
}

static bool *tree_state_lookup(ecs_entity_t entity)
{
	struct tree_state_entry *states = get_tree_state_table();
	int *count = get_tree_state_count();
	for (int i = 0; i < *count; i++) {
		if (states[i].entity == entity) {
			return &states[i].expanded;
		}
	}
	if (*count >= TREE_STATE_CAPACITY) {
		return NULL;
	}
	states[*count].entity = entity;
	states[*count].expanded = false;
	return &states[(*count)++].expanded;
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

static int draw_tree_row(mu_Context *mu, ecs_entity_t entity, const char *label,
			 int level, bool has_children, bool expanded,
			 bool *selected)
{
	int row_h = mu->style->size.y + (mu->style->padding * 2);
	mu_Rect r;
	if (level > 0) {
		int widths[2] = {level * mu->style->indent, -1};
		mu_layout_row(mu, 2, widths, row_h);
		mu_layout_next(mu);
		r = mu_layout_next(mu);
	} else {
		int width = -1;
		mu_layout_row(mu, 1, &width, row_h);
		r = mu_layout_next(mu);
	}

	mu_push_id(mu, &entity, sizeof(entity));
	mu_Id icon_id = mu_get_id(mu, "icon", 4);
	mu_Id row_id = mu_get_id(mu, "row", 3);
	mu_pop_id(mu);

	mu_Rect icon_r = mu_rect(r.x, r.y, r.h, r.h);
	mu_Rect row_r = r;
	if (has_children) {
		row_r.x += r.h;
		row_r.w -= r.h;
	}

	int flags = 0;

	if (has_children) {
		mu_update_control(mu, icon_id, icon_r, 0);
		if (mu->mouse_pressed == MU_MOUSE_LEFT &&
		    mu->focus == icon_id) {
			flags |= TREE_ROW_ICON_CLICKED;
		}
	}

	mu_update_control(mu, row_id, row_r, 0);
	if (mu->mouse_pressed == MU_MOUSE_LEFT && mu->focus == row_id) {
		*selected = (bool)(!*selected);
		flags |= TREE_ROW_CLICKED;
	}

	if (*selected) {
		mu_draw_rect(mu, r, mu_color(65, 105, 225, 160));
	} else if (mu->hover == row_id) {
		mu_draw_rect(mu, r, mu_color(255, 255, 255, 24));
	}

	if (has_children) {
		mu_draw_icon(
		    mu, (int)expanded ? MU_ICON_EXPANDED : MU_ICON_COLLAPSED,
		    icon_r, mu->style->colors[MU_COLOR_TEXT]);
	}

	mu_Rect text_r = mu_rect(r.x + r.h + mu->style->padding, r.y,
				 r.w - r.h - mu->style->padding, r.h);
	mu_draw_control_text(mu, label, text_r, MU_COLOR_TEXT, 0);

	return flags;
}

static void handle_tree_selection(CoreContext *ctx, ecs_entity_t entity,
				  const char *name, bool selected)
{
	if (selected) {
		ctx->selected_entity = entity;
		log_printf(LOG_LEVEL_INFO, "selected '%s' (id %llu)", name,
			   (unsigned long long)entity);
	} else {
		ctx->selected_entity = 0;
		log_printf(LOG_LEVEL_INFO, "deselected '%s' (id %llu)", name,
			   (unsigned long long)entity);
	}
}

static void push_children(struct node_task *stack, int *depth,
			  ecs_entity_t *children, int child_count, int level)
{
	node_task_push(stack, depth, 0, level, true);
	for (int i = child_count - 1; i >= 0; i--) {
		node_task_push(stack, depth, children[i], level + 1, false);
	}
}

static void process_tree_node(CoreContext *ctx, struct node_task *stack,
			      int *depth, ecs_entity_t entity, int level)
{
	mu_Context *mu = ctx->mu_ctx;
	const char *name = node_name(ctx->world, entity);

	ecs_entity_t children[NODE_STACK_CAPACITY];
	int child_count = collect_gameobject_children(ctx, entity, children,
						      NODE_STACK_CAPACITY);
	bool has_children = child_count > 0;

	bool *expanded_ptr = tree_state_lookup(entity);
	bool expanded = false;
	if (expanded_ptr != NULL) {
		expanded = *expanded_ptr;
	}
	bool selected = (ctx->selected_entity == entity);

	int flags = draw_tree_row(mu, entity, name, level, has_children,
				  expanded, &selected);

	if ((flags & TREE_ROW_ICON_CLICKED) && expanded_ptr != NULL) {
		*expanded_ptr = (bool)(!*expanded_ptr);
		expanded = *expanded_ptr;
	}

	if (flags & TREE_ROW_CLICKED) {
		handle_tree_selection(ctx, entity, name, selected);
	}

	if (expanded && has_children &&
	    *depth + 1 + child_count < NODE_STACK_CAPACITY) {
		push_children(stack, depth, children, child_count, level);
	}
}

static void draw_entity_tree(CoreContext *ctx, ecs_entity_t root)
{
	struct node_task stack[NODE_STACK_CAPACITY];
	int depth = 0;
	if (!node_task_push(stack, &depth, root, 0, false)) {
		return;
	}

	while (depth > 0) {
		struct node_task task = stack[--depth];
		if (task.close) {
			continue;
		}
		process_tree_node(ctx, stack, &depth, task.entity, task.level);
	}
}

void gui_panel_hierarchy(CoreContext *ctx)
{
	mu_Context *mu = ctx->mu_ctx;
	mu_begin_panel_ex(mu, "Hierarchy", 0);

	int width = -1;
	mu_layout_row(mu, 1, &width, 0);

	if (mu_header(mu, "RootScene")) {
		ecs_iter_t it =
		    ecs_query_iter(ctx->world, ctx->hierarchy_query);
		while (ecs_query_next(&it)) {
			for (int i = 0; i < it.count; i++) {
				draw_entity_tree(ctx, it.entities[i]);
			}
		}
	}

	mu_end_panel(mu);
}
