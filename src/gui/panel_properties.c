#include "gui/panel_properties.h"
#include "components.h"
#include "raylib-nuklear.h"

static void draw_vec3_property(struct nk_context *nk, const char *label,
                               Vector3 *vec, float min, float max, float step) {
  nk_layout_row_dynamic(nk, 20, 1);
  nk_label(nk, label, NK_TEXT_LEFT);

  char buf_x[64];
  char buf_y[64];
  char buf_z[64];
  (void)snprintf(buf_x, sizeof(buf_x), "%s X", label);
  (void)snprintf(buf_y, sizeof(buf_y), "%s Y", label);
  (void)snprintf(buf_z, sizeof(buf_z), "%s Z", label);

  nk_layout_row_dynamic(nk, 25, 1);
  nk_property_float(nk, buf_x, min, &vec->x, max, step, step * 0.5f);
  nk_property_float(nk, buf_y, min, &vec->y, max, step, step * 0.5f);
  nk_property_float(nk, buf_z, min, &vec->z, max, step, step * 0.5f);
}

static void draw_transform_section(struct nk_context *nk, Transform3D *t) {
  if (nk_tree_push(nk, NK_TREE_TAB, "Transform3D", NK_MAXIMIZED)) {
    draw_vec3_property(nk, "Position", &t->position, -1000.0f, 1000.0f, 0.1f);
    draw_vec3_property(nk, "Rotation", &t->rotation, -360.0f, 360.0f, 1.0f);
    draw_vec3_property(nk, "Scale", &t->scale, -100.0f, 100.0f, 0.1f);
    nk_tree_pop(nk);
  }
}

static void draw_mesh_renderer_section(struct nk_context *nk,
                                       MeshRenderer *mr) {
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

void gui_panel_properties(CoreContext *ctx) {
  if (nk_group_begin(ctx->nk_ctx, "Properties",
                     NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    if (ctx->selected_entity == 0) {
      nk_layout_row_dynamic(ctx->nk_ctx, 20, 1);
      nk_label(ctx->nk_ctx, "No entity selected", NK_TEXT_CENTERED);
      nk_group_end(ctx->nk_ctx);
      return;
    }

    // entity name header
    const char *name = ecs_get_name(ctx->world, ctx->selected_entity);
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
