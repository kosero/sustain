#include "sustain/graphics/sprite.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void sprite_init_pool(SpriteManager *mgr) {
  if (!mgr)
    return;

  mgr->active_count = 0;
  mgr->debug_flags = SPRITE_DEBUG_NONE;
  mgr->debug_color_box = GREEN;
  mgr->debug_color_origin = RED;

  for (int i = 0; i < MAX_SPRITES; i++) {
    mgr->pool[i].active = false;
  }
}

Sprite *sprite_create(SpriteManager *mgr, Texture2D *tex, const char *name) {
  if (!mgr || !tex)
    return NULL;

  for (int i = 0; i < MAX_SPRITES; i++) {
    if (!mgr->pool[i].active) {
      Sprite *s = &mgr->pool[i];

      if (name) {
        strncpy(s->name, name, SPRITE_NAME_MAX - 1);
        s->name[SPRITE_NAME_MAX - 1] = '\0';
      } else {
        snprintf(s->name, SPRITE_NAME_MAX, "Sprite_%d", i);
      }

      s->texture = *tex;
      s->position = (Vector2){0.0f, 0.0f};
      s->origin = (Vector2){0.5f, 0.5f};
      s->offset = (Vector2){0.0f, 0.0f};
      s->scale = 1.0f;
      s->rotation = 0.0f;
      s->z_index = 0;

      s->flip_h = false;
      s->flip_v = false;
      s->visible = true;
      s->alpha = 255;

      s->active = true;
      s->is_dirty = true;

      mgr->active_count++;
      return s;
    }
  }

  TraceLog(LOG_WARNING, "SPRITE: Pool is full! Cannot create '%s'",
           name ? name : "unnamed");
  return NULL;
}

void sprite_update_transform(Sprite *s) {
  if (!s)
    return;

  float src_width = (float)s->texture.width;
  float src_height = (float)s->texture.height;

  s->source_rect =
      (Rectangle){0.0f, 0.0f, src_width * (s->flip_h ? -1.0f : 1.0f),
                  src_height * (s->flip_v ? -1.0f : 1.0f)};

  float dest_width = src_width * s->scale;
  float dest_height = src_height * s->scale;

  s->dest_rect =
      (Rectangle){s->position.x + s->offset.x, s->position.y + s->offset.y,
                  dest_width, dest_height};

  if (s->dest_rect.width < 0)
    s->dest_rect.width *= -1;
  if (s->dest_rect.height < 0)
    s->dest_rect.height *= -1;

  s->origin_point =
      (Vector2){dest_width * s->origin.x, dest_height * s->origin.y};

  s->tint = (Color){255, 255, 255, s->alpha};
  s->is_dirty = false;
}

void sprite_render_all(SpriteManager *mgr) {
  if (!mgr)
    return;

  for (int i = 0; i < MAX_SPRITES; i++) {
    Sprite *s = &mgr->pool[i];
    if (s->active && s->visible) {

      // Lazy update
      if (s->is_dirty) {
        sprite_update_transform(s);
      }

      DrawTexturePro(s->texture, s->source_rect, s->dest_rect, s->origin_point,
                     s->rotation, s->tint);
    }
  }
}

void sprite_render_debug(SpriteManager *mgr) {
  if (!mgr || mgr->debug_flags == SPRITE_DEBUG_NONE)
    return;

  for (int i = 0; i < MAX_SPRITES; i++) {
    Sprite *s = &mgr->pool[i];
    if (s->active && s->visible) {

      if (mgr->debug_flags & SPRITE_DEBUG_ORIGIN) {
        DrawCircleV(s->position, 3.0f, mgr->debug_color_origin);
        DrawLine(s->position.x - 5, s->position.y, s->position.x + 5,
                 s->position.y, mgr->debug_color_origin);
        DrawLine(s->position.x, s->position.y - 5, s->position.x,
                 s->position.y + 5, mgr->debug_color_origin);
      }

      if (mgr->debug_flags & SPRITE_DEBUG_BOX) {
        if (s->rotation == 0.0f) {
          Rectangle debug_rect = s->dest_rect;
          debug_rect.x -= s->origin_point.x;
          debug_rect.y -= s->origin_point.y;
          DrawRectangleLinesEx(debug_rect, 1.0f, mgr->debug_color_box);
        } else {
          DrawCircleLines(s->position.x, s->position.y,
                          s->dest_rect.width * 0.5f, mgr->debug_color_box);
        }
      }

      if (mgr->debug_flags & SPRITE_DEBUG_NAME) {
        DrawText(s->name, (int)s->position.x + 10, (int)s->position.y - 10, 10,
                 RED);
      }
    }
  }
}

void sprite_set_debug_mode(SpriteManager *mgr, SpriteDebugFlags flags) {
  if (mgr) {
    mgr->debug_flags = flags;
  }
}
