#include "editor_state.h"
#include <string.h>

static EditorState state;

void EditorState_Init(void) {
  state.activeMenuIndex = -1;
  state.activeTool = 0;

  state.sceneOffset = (Vector2){0, 0};
  state.zoomLevel = 1.0f;
  state.isDraggingScene = false;

  strcpy(state.objectNameBuffer, "obj 1");
  state.nameEditMode = false;
  state.visibleChecked = true;
  state.transformX = 0;
  state.transformY = 0;

  state.hierarchyScrollIndex = 0;
  state.hierarchyActiveItem = 0;

  state.activeTab = 0;

  state.shouldExit = false;
}

EditorState *EditorState_Get(void) { return &state; }
