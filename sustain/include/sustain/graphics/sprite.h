#ifndef SUSTAIN_GRAPHICS_SPRITE_H
#define SUSTAIN_GRAPHICS_SPRITE_H

#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_SPRITES 2000
#define SPRITE_NAME_MAX 32

// Debug flags for sprite rendering
typedef enum SpriteDebugFlags {
    SPRITE_DEBUG_NONE       = 0,
    SPRITE_DEBUG_BOX        = 1 << 0, // Draw bounding box
    SPRITE_DEBUG_ORIGIN     = 1 << 1, // Draw origin point
    SPRITE_DEBUG_NAME       = 1 << 2, // Draw sprite name
    SPRITE_DEBUG_ALL        = 0xFF
} SpriteDebugFlags;

typedef struct Sprite {
    char name[SPRITE_NAME_MAX]; // Useful for debugging
    Texture2D texture;
    
    // Transform
    Vector2 position;
    Vector2 origin;     // Normalized (0.0 - 1.0)
    Vector2 offset;     // Pixel offset
    float scale;
    float rotation;
    int z_index;        // For rendering order
    
    // State
    bool flip_h;
    bool flip_v;
    bool visible;
    bool active;
    bool is_dirty;      // Transform needs recalculation
    uint8_t alpha;

    // Cached internal transform data (calculated when dirty)
    Rectangle source_rect;  // Source rectangle in texture
    Rectangle dest_rect;    // Destination rectangle on screen
    Vector2 origin_point;   // Origin in pixels relative to dest_rect
    Color tint;             // Multiplied color
    
} Sprite;

typedef struct SpriteManager {
    Sprite pool[MAX_SPRITES];
    uint32_t active_count;
    
    // Debug settings
    SpriteDebugFlags debug_flags;
    Color debug_color_box;
    Color debug_color_origin;
} SpriteManager;

// Initialization
void sprite_init_pool(SpriteManager *mgr);

// Creation
Sprite* sprite_create(SpriteManager *mgr, Texture2D *tex, const char* name);

// Logic
void sprite_update_transform(Sprite *s);

// Rendering
void sprite_render_all(SpriteManager *mgr);
void sprite_render_debug(SpriteManager *mgr);

// Debug control
void sprite_set_debug_mode(SpriteManager *mgr, SpriteDebugFlags flags);

#endif // SUSTAIN_GRAPHICS_SPRITE_H
