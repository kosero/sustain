#ifndef SUSTAIN_GRAPHICS_SPRITE_H
#define SUSTAIN_GRAPHICS_SPRITE_H

#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_SPRITES 2000
#define SPRITE_NAME_MAX 32

typedef enum SpriteDebugFlags {
    SPRITE_DEBUG_NONE       = 0,
    SPRITE_DEBUG_BOX        = 1 << 0,
    SPRITE_DEBUG_ORIGIN     = 1 << 1,
    SPRITE_DEBUG_NAME       = 1 << 2,
    SPRITE_DEBUG_ALL        = 0xFF
} SpriteDebugFlags;

typedef struct Sprite {
    char name[SPRITE_NAME_MAX];
    Texture2D texture;

    Vector2 position;
    Vector2 origin;
    Vector2 offset;
    float scale;
    float rotation;
    int z_index;

    bool flip_h;
    bool flip_v;
    bool visible;
    bool active;
    bool is_dirty;
    uint8_t alpha;

    Rectangle source_rect;
    Rectangle dest_rect;
    Vector2 origin_point;
    Color tint;

} Sprite;

typedef struct SpriteManager {
    Sprite pool[MAX_SPRITES];
    uint32_t active_count;

    SpriteDebugFlags debug_flags;
    Color debug_color_box;
    Color debug_color_origin;
} SpriteManager;

void sprite_init_pool(SpriteManager *mgr);
Sprite* sprite_create(SpriteManager *mgr, Texture2D *tex, const char* name);
void sprite_update_transform(Sprite *s);
void sprite_render_all(SpriteManager *mgr);
void sprite_render_debug(SpriteManager *mgr);
void sprite_set_debug_mode(SpriteManager *mgr, SpriteDebugFlags flags);

#endif // SUSTAIN_GRAPHICS_SPRITE_H
