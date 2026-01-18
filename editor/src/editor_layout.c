#include "editor_layout.h"

static EditorLayout layout;

void EditorLayout_Update(int screenW, int screenH) {
  int topBarHeight = 24;
  int toolBarHeight = 32;
  int leftPanelWidth = 250;
  int rightPanelWidth = 300;
  int bottomPanelHeight = 220;

  layout.topbar = (Rectangle){0, 0, (float)screenW, (float)topBarHeight};

  layout.toolbar =
      (Rectangle){0, (float)topBarHeight, (float)screenW, (float)toolBarHeight};

  float mainContentY = topBarHeight + toolBarHeight;
  float mainContentH = screenH - mainContentY - bottomPanelHeight;

  layout.leftPanel =
      (Rectangle){0, mainContentY, (float)leftPanelWidth, mainContentH};

  layout.rightPanel =
      (Rectangle){(float)(screenW - rightPanelWidth), mainContentY,
                  (float)rightPanelWidth, mainContentH};

  layout.bottomPanel = (Rectangle){0, (float)(screenH - bottomPanelHeight),
                                   (float)screenW, (float)bottomPanelHeight};

  layout.centerView = (Rectangle){
      (float)leftPanelWidth, mainContentY,
      (float)(screenW - leftPanelWidth - rightPanelWidth), mainContentH};
}

EditorLayout *EditorLayout_Get(void) { return &layout; }
