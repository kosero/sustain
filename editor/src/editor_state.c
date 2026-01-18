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
  state.hierarchyActiveItem = -1;
  state.showHierarchyContextMenu = false;
  state.hierarchyContextMenuPos = (Vector2){0, 0};

  state.hierarchyNodeCount = 7;
  strcpy(state.hierarchyNodeNames[0], "Scene Root");
  strcpy(state.hierarchyNodeNames[1], "Map Layer 1");
  strcpy(state.hierarchyNodeNames[2], "Map Layer 2");
  strcpy(state.hierarchyNodeNames[3], "Objects");
  strcpy(state.hierarchyNodeNames[4], "Player");
  strcpy(state.hierarchyNodeNames[5], "Enemy_Slime");
  strcpy(state.hierarchyNodeNames[6], "UI Layer");

  state.hasClipboardData = false;
  state.clipboardIsCut = false;

  state.activeTab = 0;

  state.shouldExit = false;
  state.showGridLines = true;
}

EditorState *EditorState_Get(void) { return &state; }
