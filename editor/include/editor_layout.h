#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include <raylib.h>

typedef struct {
  Rectangle topbar;
  Rectangle toolbar;
  Rectangle leftPanel;
  Rectangle rightPanel;
  Rectangle bottomPanel;
  Rectangle centerView;
} EditorLayout;

void EditorLayout_Update(int screenW, int screenH);
EditorLayout *EditorLayout_Get(void);

#endif
