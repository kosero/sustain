#include "sustain/core/window.h"
#include "sustain/graphics/sprite.h"
#include <raylib.h>
#include <sys/stat.h>

static Texture2D tex;
static SpriteManager smgr;
static int shouldExit = 0;

static void update(void) {}
static void render(void) {
  ClearBackground(BLACK);
  sprite_render_all(&smgr);
  sprite_render_debug(&smgr);
}

int main(void) {
  window_init(1024, 800, "Sustain Editor", 0);

  sprite_init_pool(&smgr);
  sprite_set_debug_mode(&smgr, SPRITE_DEBUG_BOX | SPRITE_DEBUG_NAME |
                                   SPRITE_DEBUG_ORIGIN);

  tex = LoadTexture("assets/imgs/cube.png");
  Sprite *player = sprite_create(&smgr, &tex, "Player");
  player->scale = 5;
  player->position = (Vector2){100, 100};
  player->is_dirty = true;

  while (!WindowShouldClose() && !shouldExit) {
    update();

    BeginDrawing();
    render();
    EndDrawing();
  }

  window_close();
  return 0;
}
