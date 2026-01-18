#include "sustain/ui/raygui.h"
#include <raylib.h>

#include "editor_layout.h"
#include "editor_state.h"
#include "gui_panels.h"

extern void window_init(int width, int height, const char *title,
                        int target_fps);
extern void window_close(void);

static void update(void) {
  EditorState *state = EditorState_Get();

  EditorLayout_Update(GetScreenWidth(), GetScreenHeight());

  // Close menu if clicked outside
  EditorLayout *layout = EditorLayout_Get();
  if (state->activeMenuIndex != -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Vector2 mouse = GetMousePosition();
    if (mouse.y > layout->topbar.height &&
        mouse.y < layout->topbar.height + 150) {
      bool overMenu = (mouse.x >= 0 && mouse.x <= 250 && mouse.y <= 150);
      if (!overMenu)
        state->activeMenuIndex = -1;
    } else if (mouse.y > layout->topbar.height) {
      state->activeMenuIndex = -1;
    }
  }

  // Scene Input: Panning & Zoom
  bool mouseOverScene =
      CheckCollisionPointRec(GetMousePosition(), layout->centerView);

  if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && mouseOverScene) {
    state->isDraggingScene = true;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
    state->isDraggingScene = false;
  }

  if (state->isDraggingScene) {
    Vector2 delta = GetMouseDelta();
    state->sceneOffset.x += delta.x;
    state->sceneOffset.y += delta.y;
  }

  if (mouseOverScene) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      state->zoomLevel += wheel * 0.1f;
      if (state->zoomLevel < 0.2f)
        state->zoomLevel = 0.2f;
      if (state->zoomLevel > 5.0f)
        state->zoomLevel = 5.0f;
    }
  }
}

static void render(void) {
  EditorState *state = EditorState_Get();

  ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

  Gui_DrawSceneView();
  Gui_DrawMenuBar();

  if (state->activeMenuIndex != -1)
    GuiLock();

  Gui_DrawToolbar();
  Gui_DrawHierarchy();
  Gui_DrawInspector();
  Gui_DrawBottomPanel();
  Gui_DrawWarnMessageBox();

  if (state->activeMenuIndex != -1)
    GuiUnlock();

  Gui_DrawDropdowns();
}

int main(void) {
  window_init(1280, 800, "Sustain Editor", 0);

  EditorState_Init();
  Gui_Init(); // Loads style

  EditorState *state = EditorState_Get();

  while (!WindowShouldClose() && !state->shouldExit) {
    update();

    BeginDrawing();
    render();
    EndDrawing();
  }

  window_close();
  return 0;
}
