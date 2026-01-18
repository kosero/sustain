#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include <raylib.h>
#include <stdbool.h>

typedef struct {
  // Menu & Tools
  int activeMenuIndex;
  int activeTool;

  // Scene View
  Vector2 sceneOffset;
  float zoomLevel;
  bool isDraggingScene;

  // Inspector
  char objectNameBuffer[64];
  bool nameEditMode;
  bool visibleChecked;
  int transformX;
  int transformY;

  // Hierarchy
  int hierarchyScrollIndex;
  int hierarchyActiveItem;
  char hierarchyNodeNames[64][64];
  int hierarchyNodeCount;
  int lastSelected;
  bool showHierarchyContextMenu;
  Vector2 hierarchyContextMenuPos;

  // Project
  int activeTab; // 0=Assets, 1=Console

  // System
  bool shouldExit;
  bool showWarnMessageBox;
  char warnTitle[64];
  char warnMessage[256];
  char warnOptions[64];
} EditorState;

void EditorState_Init(void);
EditorState *EditorState_Get(void);

#endif
