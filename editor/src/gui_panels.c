#include "gui_panels.h"
#include "editor_layout.h"
#include "editor_state.h"
#include "sustain/ui/raygui.h"
#include "sustain/ui/styles/style_amber.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void Gui_Init(void) {
  GuiLoadStyleAmber();
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
}

void Gui_DrawMenuBar(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  GuiPanel(layout->topbar, NULL);

  float btnWidth = 60;
  if (GuiLabelButton((Rectangle){0, 0, btnWidth, layout->topbar.height},
                     "File")) {
    state->activeMenuIndex = (state->activeMenuIndex == 0) ? -1 : 0;
  }
  if (GuiLabelButton((Rectangle){btnWidth, 0, btnWidth, layout->topbar.height},
                     "Edit")) {
    state->activeMenuIndex = (state->activeMenuIndex == 1) ? -1 : 1;
  }
  if (GuiLabelButton(
          (Rectangle){btnWidth * 2, 0, btnWidth, layout->topbar.height},
          "View")) {
    state->activeMenuIndex = (state->activeMenuIndex == 2) ? -1 : 2;
  }
  if (GuiLabelButton(
          (Rectangle){btnWidth * 3, 0, btnWidth, layout->topbar.height},
          "Game")) {
    state->activeMenuIndex = (state->activeMenuIndex == 3) ? -1 : 3;
  }
  if (GuiLabelButton(
          (Rectangle){btnWidth * 4, 0, btnWidth, layout->topbar.height},
          "Help")) {
    state->activeMenuIndex = (state->activeMenuIndex == 4) ? -1 : 4;
  }
}

void Gui_DrawToolbar(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  GuiPanel(layout->toolbar, NULL);

  const char *tools[] = {"#21#Select", "#23#Move", "#22#Brush", "#28#Erase"};
  int toolX = 5;
  for (int i = 0; i < 4; i++) {
    bool active = (state->activeTool == i);
    GuiToggle((Rectangle){(float)toolX, layout->toolbar.y + 2, 80, 28},
              tools[i], &active);
    if (active)
      state->activeTool = i;
    toolX += 85;
  }

  float playX = layout->toolbar.width / 2 - 60;
  if (GuiButton((Rectangle){playX, layout->toolbar.y + 2, 40, 28}, "#131#")) {
    Gui_ShowAlert("[WARN]", "Not implemented yet!", "OK");
  }
  if (GuiButton((Rectangle){playX + 45, layout->toolbar.y + 2, 40, 28},
                "#133#")) {
    Gui_ShowAlert("[WARN]", "Not implemented yet!", "OK");
  }
}

void Gui_DrawHierarchy(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  GuiPanel(layout->leftPanel, "Hierarchy & Tiles");

  Rectangle content = layout->leftPanel;
  content.y += 24;
  content.height -= 24;

  char listText[1024] = {0};
  for (int i = 0; i < state->hierarchyNodeCount; i++) {
    strcat(listText, state->hierarchyNodeNames[i]);
    if (i < state->hierarchyNodeCount - 1)
      strcat(listText, ";");
  }

  GuiListView((Rectangle){content.x + 5, content.y + 5, content.width - 10,
                          content.height / 2 - 10},
              listText, &state->hierarchyScrollIndex,
              &state->hierarchyActiveItem);

  GuiLine(
      (Rectangle){content.x, content.y + content.height / 2, content.width, 1},
      NULL);
  GuiLabel(
      (Rectangle){content.x + 5, content.y + content.height / 2 + 5, 100, 20},
      "Tile Palette");

  GuiGrid((Rectangle){content.x + 5, content.y + content.height / 2 + 30,
                      content.width - 10, content.height / 2 - 40},
          NULL, 32, 2, NULL);
}

void Gui_DrawInspector(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  GuiPanel(layout->rightPanel, "Inspector");

  float startX = layout->rightPanel.x + 10;
  float startY = layout->rightPanel.y + 30;
  float width = layout->rightPanel.width - 20;

  const char *selectedName = "None";
  if (state->hierarchyActiveItem >= 0 &&
      state->hierarchyActiveItem < state->hierarchyNodeCount) {

    if (state->lastSelected != state->hierarchyActiveItem) {
      strncpy(state->objectNameBuffer,
              state->hierarchyNodeNames[state->hierarchyActiveItem], 63);
      state->objectNameBuffer[63] = '\0';

      state->lastSelected = state->hierarchyActiveItem;
    }

    selectedName = state->hierarchyNodeNames[state->hierarchyActiveItem];
  }

  char labelBuffer[64];
  sprintf(labelBuffer, "Selected: %s", selectedName);
  GuiLabel((Rectangle){startX, startY, width, 20}, labelBuffer);
  startY += 25;

  GuiLine((Rectangle){startX, startY, width, 1}, NULL);
  startY += 10;

  GuiLabel((Rectangle){startX, startY, 60, 20}, "Name:");
  Rectangle textBoxBounds = {startX + 50, startY, width - 50, 20};

  if (GuiTextBox(textBoxBounds, state->objectNameBuffer, 64,
                 state->nameEditMode)) {
    state->nameEditMode = !state->nameEditMode;
    if (!state->nameEditMode) {
      strncpy(state->hierarchyNodeNames[state->hierarchyActiveItem],
              state->objectNameBuffer, 63);
      state->hierarchyNodeNames[state->hierarchyActiveItem][63] = '\0';
    }
  }

  if (state->nameEditMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (!CheckCollisionPointRec(GetMousePosition(), textBoxBounds)) {
      state->nameEditMode = false;
    }
  }
  startY += 30;

  GuiCheckBox((Rectangle){startX, startY, 20, 20}, "Visible",
              &state->visibleChecked);
  startY += 30;

  GuiLabel((Rectangle){startX, startY, width, 20}, "Transform");
  startY += 20;
  GuiLabel((Rectangle){startX + 10, startY, 20, 20}, "X:");
  GuiSpinner((Rectangle){startX + 30, startY, 80, 20}, NULL, &state->transformX,
             0, 1000, false);
  GuiLabel((Rectangle){startX + 120, startY, 20, 20}, "Y:");
  GuiSpinner((Rectangle){startX + 140, startY, 80, 20}, NULL,
             &state->transformY, 0, 1000, false);
  startY += 30;

  GuiLine((Rectangle){startX, startY, width, 1}, NULL);
  startY += 10;

  GuiLabel((Rectangle){startX, startY, width, 20}, "Script Component");
  startY += 25;

  if (GuiButton((Rectangle){startX, startY, width, 24}, "Open Script")) {
    Gui_ShowAlert("[WARN]", "Not implemented yet!", "OK");
  }
}

void Gui_DrawBottomPanel(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  GuiPanel(layout->bottomPanel, "Project & Console");

  Rectangle content = layout->bottomPanel;
  content.y += 24;

  float tabW = 80;
  if (GuiButton((Rectangle){content.x, content.y, tabW, 24}, "Assets"))
    state->activeTab = 0;
  if (GuiButton((Rectangle){content.x + tabW, content.y, tabW, 24}, "Console"))
    state->activeTab = 1;

  Rectangle view = {content.x + 5, content.y + 30, content.width - 10,
                    content.height - 60};
  GuiGroupBox(view, state->activeTab == 0 ? "File Browser" : "Log Output");

  if (state->activeTab == 0) {
    GuiGrid(view, NULL, 50, 3, NULL);
  } else {
    GuiLabel((Rectangle){view.x + 5, view.y + 5, view.width, 20},
             "[INFO] Editor initialized successfully.");
    GuiLabel((Rectangle){view.x + 5, view.y + 25, view.width, 20},
             "[WARN] No project loaded.");
  }
}

void Gui_DrawSceneView(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  DrawRectangleRec(layout->centerView, (Color){40, 40, 40, 255});

  BeginScissorMode((int)layout->centerView.x, (int)layout->centerView.y,
                   (int)layout->centerView.width,
                   (int)layout->centerView.height);

  int spacing = (int)(32 * state->zoomLevel);
  if (spacing < 4)
    spacing = 4;

  int cols = (int)(layout->centerView.width / spacing) + 2;
  int rows = (int)(layout->centerView.height / spacing) + 2;

  int offX = (int)state->sceneOffset.x % spacing;
  int offY = (int)state->sceneOffset.y % spacing;

  Color gridColor = Fade(WHITE, 0.1f);

  int startX = (int)layout->centerView.x;
  int startY = (int)layout->centerView.y;
  int endX = startX + (int)layout->centerView.width;
  int endY = startY + (int)layout->centerView.height;

  for (int i = -1; i < cols; i++) {
    int x = startX + i * spacing + offX;
    DrawLine(x, startY, x, endY, gridColor);
  }

  for (int i = -1; i < rows; i++) {
    int y = startY + i * spacing + offY;
    DrawLine(startX, y, endX, y, gridColor);
  }

  if (state->isDraggingScene) {
    char buff[64];
    sprintf(buff, "Offset: %.0f, %.0f | Zoom: %.1fx", state->sceneOffset.x,
            state->sceneOffset.y, state->zoomLevel);
    DrawText(buff, startX + 10, startY + 10, 20, GRAY);
  }

  EndScissorMode();

  DrawRectangleLinesEx(layout->centerView, 1, Fade(WHITE, 0.2f));
}

void Gui_DrawDropdowns(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  if (state->activeMenuIndex == -1)
    return;

  float btnWidth = 60;
  float itemHeight = 24;
  float menuWidth = 120;
  float startX = state->activeMenuIndex * btnWidth;
  float startY = layout->topbar.height;

  const char *fileItems[] = {"New Project", "Open...", "Save", "Save As...",
                             "Exit"};
  const char *editItems[] = {"Undo", "Redo", "Cut", "Copy", "Paste"};
  const char *viewItems[] = {"Zoom In", "Zoom Out", "Reset Zoom",
                             "Toggle Grid"};
  const char *gameItems[] = {"Play Scene", "Stop", "Build & Run"};
  const char *helpItems[] = {"Documentation", "About Sustain"};

  int count = 0;
  const char **items = NULL;

  switch (state->activeMenuIndex) {
  case 0:
    items = fileItems;
    count = 5;
    break;
  case 1:
    items = editItems;
    count = 5;
    break;
  case 2:
    items = viewItems;
    count = 4;
    break;
  case 3:
    items = gameItems;
    count = 3;
    break;
  case 4:
    items = helpItems;
    count = 2;
    break;
  }

  if (items) {
    Rectangle bounds = {startX, startY, menuWidth, count * itemHeight + 2};
    GuiPanel(bounds, NULL);

    for (int i = 0; i < count; i++) {
      if (GuiLabelButton((Rectangle){startX + 2, startY + 1 + i * itemHeight,
                                     menuWidth - 4, itemHeight},
                         items[i])) {
        state->activeMenuIndex = -1;
        Gui_ShowAlert("[WARN]", "Not implemented yet!", "OK");
      }
    }
    DrawRectangleLinesEx(bounds, 1, Fade(BLACK, 0.5f));
  }
}

void Gui_ShowAlert(const char *title, const char *message,
                   const char *options) {
  EditorState *state = EditorState_Get();
  strncpy(state->warnTitle, title, 63);
  strncpy(state->warnMessage, message, 255);
  strncpy(state->warnOptions, options, 63);
  state->showWarnMessageBox = true;
}

int Gui_DrawWarnMessageBox(void) {
  EditorState *state = EditorState_Get();

  if (!state->showWarnMessageBox)
    return -1;

  int result =
      GuiMessageBox((Rectangle){GetScreenWidth() / 2.0f - 150,
                                GetScreenHeight() / 2.0f - 60, 300, 120},
                    state->warnTitle, state->warnMessage, state->warnOptions);

  if (result >= 0) {
    state->showWarnMessageBox = false;
  }
  return result;
}
