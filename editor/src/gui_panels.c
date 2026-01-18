#include "gui_panels.h"
#include "editor_layout.h"
#include "editor_state.h"
#define RAYGUI_IMPLEMENTATION
#include "sustain/ui/raygui.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void Gui_Init(void) {
  GuiLoadStyleAmber();
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
  GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
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

  Rectangle listViewBounds = {content.x + 5, content.y + 5, content.width - 10,
                              content.height / 2 - 10};

  GuiListView(listViewBounds, listText, &state->hierarchyScrollIndex,
              &state->hierarchyActiveItem);

  // Right-click detection on the list view
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, listViewBounds)) {
      if (state->hierarchyActiveItem >= 0 &&
          state->hierarchyActiveItem < state->hierarchyNodeCount) {
        state->showHierarchyContextMenu = true;
        state->hierarchyContextMenuPos = mousePos;
      }
    }
  }

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

  bool hasValidSelection =
      (state->hierarchyActiveItem >= 0 &&
       state->hierarchyActiveItem < state->hierarchyNodeCount);

  if (!hasValidSelection) {
    return;
  }

  if (state->lastSelected != state->hierarchyActiveItem) {
    strncpy(state->objectNameBuffer,
            state->hierarchyNodeNames[state->hierarchyActiveItem], 63);
    state->objectNameBuffer[63] = '\0';
    state->lastSelected = state->hierarchyActiveItem;
  }

  float startX = layout->rightPanel.x + 10;
  float startY = layout->rightPanel.y + 30;
  float width = layout->rightPanel.width - 20;

  char labelBuffer[64];
  sprintf(labelBuffer, "Selected: %s",
          state->hierarchyNodeNames[state->hierarchyActiveItem]);
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

  // Only draw grid if enabled
  if (state->showGridLines) {
    for (int i = -1; i < cols; i++) {
      int x = startX + i * spacing + offX;
      DrawLine(x, startY, x, endY, gridColor);
    }

    for (int i = -1; i < rows; i++) {
      int y = startY + i * spacing + offY;
      DrawLine(startX, y, endX, y, gridColor);
    }
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
  const char *editItems[] = {"Delete", "---", "Cut", "Copy", "Paste"};
  const char *viewItems[] = {"Zoom In", "Zoom Out", "Reset Zoom",
                             "Toggle Grid"};
  const char *gameItems[] = {"Play Scene", "Stop", "Build & Run"};
  const char *helpItems[] = {"About Sustain"};

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
    count = 1;
    break;
  }

  if (items) {
    Rectangle bounds = {startX, startY, menuWidth, count * itemHeight + 2};
    GuiPanel(bounds, NULL);

    for (int i = 0; i < count; i++) {
      Rectangle itemBounds = {startX + 2, startY + 1 + i * itemHeight,
                              menuWidth - 4, itemHeight};

      if (strcmp(items[i], "---") == 0) {
        GuiLine((Rectangle){itemBounds.x, itemBounds.y + itemHeight / 2,
                            itemBounds.width, 1},
                NULL);
        continue;
      }

      if (GuiLabelButton(itemBounds, items[i])) {
        int menuIndex = state->activeMenuIndex;
        state->activeMenuIndex = -1;

        // File menu
        if (menuIndex == 0) {
          if (i == 4) { // Exit
            state->shouldExit = true;
          } else {
            Gui_ShowAlert("[INFO]", "Not implemented yet!", "OK");
          }
        }
        // Edit menu
        else if (menuIndex == 1) {
          switch (i) {
          case 0: // Delete
            if (state->hierarchyActiveItem >= 0 &&
                state->hierarchyActiveItem < state->hierarchyNodeCount) {
              for (int j = state->hierarchyActiveItem;
                   j < state->hierarchyNodeCount - 1; j++) {
                strcpy(state->hierarchyNodeNames[j],
                       state->hierarchyNodeNames[j + 1]);
              }
              state->hierarchyNodeCount--;
              if (state->hierarchyActiveItem >= state->hierarchyNodeCount) {
                state->hierarchyActiveItem = state->hierarchyNodeCount - 1;
              }
              // Clear clipboard if we deleted the cut item
              if (state->clipboardIsCut) {
                state->hasClipboardData = false;
                state->clipboardIsCut = false;
              }
            }
            break;
          case 2: // Cut
            if (state->hierarchyActiveItem >= 0 &&
                state->hierarchyActiveItem < state->hierarchyNodeCount) {
              strcpy(state->clipboardBuffer,
                     state->hierarchyNodeNames[state->hierarchyActiveItem]);
              state->hasClipboardData = true;
              state->clipboardIsCut = true;
            }
            break;
          case 3: // Copy
            if (state->hierarchyActiveItem >= 0 &&
                state->hierarchyActiveItem < state->hierarchyNodeCount) {
              strcpy(state->clipboardBuffer,
                     state->hierarchyNodeNames[state->hierarchyActiveItem]);
              state->hasClipboardData = true;
              state->clipboardIsCut = false;
            }
            break;
          case 4: // Paste
            if (state->hasClipboardData && state->hierarchyNodeCount < 64) {
              // If cut, delete the original
              if (state->clipboardIsCut) {
                // Find and delete the original
                for (int j = 0; j < state->hierarchyNodeCount; j++) {
                  if (strcmp(state->hierarchyNodeNames[j],
                             state->clipboardBuffer) == 0) {
                    for (int k = j; k < state->hierarchyNodeCount - 1; k++) {
                      strcpy(state->hierarchyNodeNames[k],
                             state->hierarchyNodeNames[k + 1]);
                    }
                    state->hierarchyNodeCount--;
                    break;
                  }
                }
                state->clipboardIsCut = false;
              }
              // Paste (add copy or moved item)
              strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount],
                     state->clipboardBuffer);
              state->hierarchyActiveItem = state->hierarchyNodeCount;
              state->hierarchyNodeCount++;
            }
            break;
          }
        }
        // View menu
        else if (menuIndex == 2) {
          switch (i) {
          case 0: // Zoom In
            state->zoomLevel += 0.2f;
            if (state->zoomLevel > 5.0f)
              state->zoomLevel = 5.0f;
            break;
          case 1: // Zoom Out
            state->zoomLevel -= 0.2f;
            if (state->zoomLevel < 0.2f)
              state->zoomLevel = 0.2f;
            break;
          case 2: // Reset Zoom
            state->zoomLevel = 1.0f;
            break;
          case 3: // Toggle Grid
            state->showGridLines = !state->showGridLines;
            break;
          }
        }
        // Game menu
        else if (menuIndex == 3) {
          Gui_ShowAlert("[INFO]", "Not implemented yet!", "OK");
        }
        // Help menu
        else if (menuIndex == 4) {
          if (i == 0) { // About Sustain
            Gui_ShowAlert("#191# About Sustain",
                          "Sustain Engine Editor\n"
                          "Version 0.1.0 Alpha\n\n"
                          "A 2D game engine and editor",
                          "OK");
          }
        }
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

void Gui_DrawHierarchyContextMenu(void) {
  EditorState *state = EditorState_Get();

  if (!state->showHierarchyContextMenu)
    return;

  const char *menuItems[] = {"Create Empty", "Delete", "Rename", "Duplicate"};
  int itemCount = 4;
  float menuWidth = 140;
  float itemHeight = 24;
  float menuHeight = itemCount * itemHeight + 4;

  Rectangle menuBounds = {state->hierarchyContextMenuPos.x,
                          state->hierarchyContextMenuPos.y, menuWidth,
                          menuHeight};

  GuiPanel(menuBounds, NULL);

  for (int i = 0; i < itemCount; i++) {
    Rectangle itemBounds = {menuBounds.x + 2, menuBounds.y + 2 + i * itemHeight,
                            menuWidth - 4, itemHeight};

    if (GuiLabelButton(itemBounds, menuItems[i])) {
      switch (i) {
      case 0: // Create Empty
        if (state->hierarchyNodeCount < 64) {
          char newName[64];
          sprintf(newName, "New Object %d", state->hierarchyNodeCount);
          strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount], newName);
          state->hierarchyNodeCount++;
        }
        state->showHierarchyContextMenu = false;
        break;

      case 1: // Delete
        if (state->hierarchyActiveItem >= 0 &&
            state->hierarchyActiveItem < state->hierarchyNodeCount) {
          for (int j = state->hierarchyActiveItem;
               j < state->hierarchyNodeCount - 1; j++) {
            strcpy(state->hierarchyNodeNames[j],
                   state->hierarchyNodeNames[j + 1]);
          }
          state->hierarchyNodeCount--;
          if (state->hierarchyActiveItem >= state->hierarchyNodeCount) {
            state->hierarchyActiveItem = state->hierarchyNodeCount - 1;
          }
        }
        state->showHierarchyContextMenu = false;
        break;

      case 2: // Rename
        if (state->hierarchyActiveItem >= 0 &&
            state->hierarchyActiveItem < state->hierarchyNodeCount) {
          state->nameEditMode = true;
        }
        state->showHierarchyContextMenu = false;
        break;

      case 3: // Duplicate
        if (state->hierarchyActiveItem >= 0 &&
            state->hierarchyActiveItem < state->hierarchyNodeCount &&
            state->hierarchyNodeCount < 64) {
          char duplicateName[64];
          sprintf(duplicateName, "%s (Copy)",
                  state->hierarchyNodeNames[state->hierarchyActiveItem]);
          strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount],
                 duplicateName);
          state->hierarchyNodeCount++;
        }
        state->showHierarchyContextMenu = false;
        break;
      }
    }
  }

  DrawRectangleLinesEx(menuBounds, 1, Fade(BLACK, 0.5f));

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
      IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    Vector2 mousePos = GetMousePosition();
    if (!CheckCollisionPointRec(mousePos, menuBounds)) {
      state->showHierarchyContextMenu = false;
    }
  }
}

int Gui_DrawWarnMessageBox(void) {
  EditorState *state = EditorState_Get();

  if (!state->showWarnMessageBox)
    return -1;

  int result =
      GuiMessageBox((Rectangle){GetScreenWidth() / 2.0f - 200,
                                GetScreenHeight() / 2.0f - 100, 400, 200},
                    state->warnTitle, state->warnMessage, state->warnOptions);

  if (result >= 0) {
    state->showWarnMessageBox = false;
  }
  return result;
}
