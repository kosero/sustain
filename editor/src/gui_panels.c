#include "gui_panels.h"
#include "editor_layout.h"
#include "editor_state.h"
#include "sustain/ui/ui_system.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

static void OnFileMenuClicked(void *userData) {
  int index = *(int *)userData;
  EditorState *state = EditorState_Get();

  if (index == 4) { // Exit
    state->shouldExit = true;
  } else {
    UI_ShowAlert("[INFO]", "Not implemented yet!", "OK");
  }
}

static void OnEditMenuClicked(void *userData) {
  int index = *(int *)userData;
  EditorState *state = EditorState_Get();

  switch (index) {
  case 0: // Delete
    if (state->hierarchyActiveItem >= 0 &&
        state->hierarchyActiveItem < state->hierarchyNodeCount) {
      for (int j = state->hierarchyActiveItem;
           j < state->hierarchyNodeCount - 1; j++) {
        strcpy(state->hierarchyNodeNames[j], state->hierarchyNodeNames[j + 1]);
      }
      state->hierarchyNodeCount--;
      if (state->hierarchyActiveItem >= state->hierarchyNodeCount) {
        state->hierarchyActiveItem = state->hierarchyNodeCount - 1;
      }
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
      if (state->clipboardIsCut) {
        for (int j = 0; j < state->hierarchyNodeCount; j++) {
          if (strcmp(state->hierarchyNodeNames[j], state->clipboardBuffer) ==
              0) {
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
      strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount],
             state->clipboardBuffer);
      state->hierarchyActiveItem = state->hierarchyNodeCount;
      state->hierarchyNodeCount++;
    }
    break;
  }
}

static void OnViewMenuClicked(void *userData) {
  int index = *(int *)userData;
  EditorState *state = EditorState_Get();

  switch (index) {
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

static void OnGameMenuClicked(void *userData) {
  UI_ShowAlert("[INFO]", "Not implemented yet!", "OK");
}

static void OnHelpAbout(void *userData) {
  UI_ShowAlert("#191# About Sustain",
               "Sustain Engine Editor\\n"
               "Version 0.1.0 Alpha\\n\\n"
               "A 2D game engine and editor",
               "OK");
}

static void OnToolbarPlayClicked(void *userData) {
  UI_ShowAlert("[WARN]", "Not implemented yet!", "OK");
}

static void OnToolbarStopClicked(void *userData) {
  UI_ShowAlert("[WARN]", "Not implemented yet!", "OK");
}

static void OnScriptOpenClicked(void *userData) {
  UI_ShowAlert("[WARN]", "Not implemented yet!", "OK");
}

static void OnHierarchyCreateEmpty(void *userData) {
  EditorState *state = EditorState_Get();
  if (state->hierarchyNodeCount < 64) {
    char newName[64];
    sprintf(newName, "New Object %d", state->hierarchyNodeCount);
    strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount], newName);
    state->hierarchyNodeCount++;
  }
}

static void OnHierarchyDelete(void *userData) {
  EditorState *state = EditorState_Get();
  if (state->hierarchyActiveItem >= 0 &&
      state->hierarchyActiveItem < state->hierarchyNodeCount) {
    for (int j = state->hierarchyActiveItem; j < state->hierarchyNodeCount - 1;
         j++) {
      strcpy(state->hierarchyNodeNames[j], state->hierarchyNodeNames[j + 1]);
    }
    state->hierarchyNodeCount--;
    if (state->hierarchyActiveItem >= state->hierarchyNodeCount) {
      state->hierarchyActiveItem = state->hierarchyNodeCount - 1;
    }
  }
}

static void OnHierarchyRename(void *userData) {
  EditorState *state = EditorState_Get();
  if (state->hierarchyActiveItem >= 0 &&
      state->hierarchyActiveItem < state->hierarchyNodeCount) {
    state->nameEditMode = true;
  }
}

static void OnHierarchyDuplicate(void *userData) {
  EditorState *state = EditorState_Get();
  if (state->hierarchyActiveItem >= 0 &&
      state->hierarchyActiveItem < state->hierarchyNodeCount &&
      state->hierarchyNodeCount < 64) {
    char duplicateName[64];
    sprintf(duplicateName, "%s (Copy)",
            state->hierarchyNodeNames[state->hierarchyActiveItem]);
    strcpy(state->hierarchyNodeNames[state->hierarchyNodeCount], duplicateName);
    state->hierarchyNodeCount++;
  }
}

void Gui_Init(void) { UI_Init(); }

void Gui_DrawMenuBar(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  UI_Panel(layout->topbar, NULL);

  float btnWidth = 60;

  static int fileIdx = 0, editIdx = 1, viewIdx = 2, gameIdx = 3, helpIdx = 4;

  if (UI_LabelButton("File", (Rectangle){0, 0, btnWidth, layout->topbar.height},
                     NULL, NULL)) {
    state->activeMenuIndex = (state->activeMenuIndex == 0) ? -1 : 0;
  }
  if (UI_LabelButton("Edit",
                     (Rectangle){btnWidth, 0, btnWidth, layout->topbar.height},
                     NULL, NULL)) {
    state->activeMenuIndex = (state->activeMenuIndex == 1) ? -1 : 1;
  }
  if (UI_LabelButton(
          "View", (Rectangle){btnWidth * 2, 0, btnWidth, layout->topbar.height},
          NULL, NULL)) {
    state->activeMenuIndex = (state->activeMenuIndex == 2) ? -1 : 2;
  }
  if (UI_LabelButton(
          "Game", (Rectangle){btnWidth * 3, 0, btnWidth, layout->topbar.height},
          NULL, NULL)) {
    state->activeMenuIndex = (state->activeMenuIndex == 3) ? -1 : 3;
  }
  if (UI_LabelButton(
          "Help", (Rectangle){btnWidth * 4, 0, btnWidth, layout->topbar.height},
          NULL, NULL)) {
    state->activeMenuIndex = (state->activeMenuIndex == 4) ? -1 : 4;
  }
}

void Gui_DrawToolbar(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  UI_Panel(layout->toolbar, NULL);

  const char *tools[] = {"#21#Select", "#23#Move", "#22#Brush", "#28#Erase"};
  int toolX = 5;
  for (int i = 0; i < 4; i++) {
    bool active = (state->activeTool == i);
    UI_ToggleButton(tools[i],
                    (Rectangle){(float)toolX, layout->toolbar.y + 2, 80, 28},
                    &active);
    if (active)
      state->activeTool = i;
    toolX += 85;
  }

  float playX = layout->toolbar.width / 2 - 60;
  UI_Button("#131#", (Rectangle){playX, layout->toolbar.y + 2, 40, 28},
            OnToolbarPlayClicked, NULL);
  UI_Button("#133#", (Rectangle){playX + 45, layout->toolbar.y + 2, 40, 28},
            OnToolbarStopClicked, NULL);
}

void Gui_DrawHierarchy(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  UI_Panel(layout->leftPanel, "Hierarchy & Tiles");

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

  UI_ListView(listViewBounds, listText, &state->hierarchyScrollIndex,
              &state->hierarchyActiveItem);

  // Right-click context menu
  if (UI_IsRightClicked(listViewBounds)) {
    if (state->hierarchyActiveItem >= 0) {
      state->showHierarchyContextMenu = true;
      state->hierarchyContextMenuPos = GetMousePosition();
    } else if (state->hierarchyActiveItem < state->hierarchyNodeCount) {
      state->showHierarchyContextMenu = true;
      state->hierarchyContextMenuPos = GetMousePosition();
    }
  }

  UI_Separator(
      (Rectangle){content.x, content.y + content.height / 2, content.width, 1});
  UI_Label(
      "Tile Palette",
      (Rectangle){content.x + 5, content.y + content.height / 2 + 5, 100, 20});

  UI_GroupBox((Rectangle){content.x + 5, content.y + content.height / 2 + 30,
                          content.width - 10, content.height / 2 - 40},
              NULL);
}

void Gui_DrawInspector(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  UI_Panel(layout->rightPanel, "Inspector");

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

  UILayout layout2 =
      UI_CreateVerticalLayout(UI_GetPanelContent(layout->rightPanel), 5, 10);

  char labelBuffer[64];
  sprintf(labelBuffer, "Selected: %s",
          state->hierarchyNodeNames[state->hierarchyActiveItem]);
  UI_Label(labelBuffer, UI_NextLayoutItem(&layout2, 0, 20));

  UI_Separator(UI_NextLayoutItem(&layout2, 0, 1));

  Rectangle nameRow = UI_NextLayoutItem(&layout2, 0, 20);
  UI_Label("Name:", (Rectangle){nameRow.x, nameRow.y, 50, 20});
  Rectangle textBoxBounds = {nameRow.x + 50, nameRow.y, nameRow.width - 50, 20};

  if (UI_TextBox(textBoxBounds, state->objectNameBuffer, 64,
                 &state->nameEditMode)) {
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

  UI_Checkbox(UI_NextLayoutItem(&layout2, 20, 20), "Visible",
              &state->visibleChecked);

  UI_Label("Transform", UI_NextLayoutItem(&layout2, 0, 20));

  Rectangle transformRow = UI_NextLayoutItem(&layout2, 0, 20);
  UI_Label("X:", (Rectangle){transformRow.x + 10, transformRow.y, 20, 20});
  UI_Spinner((Rectangle){transformRow.x + 30, transformRow.y, 80, 20},
             &state->transformX, 0, 1000);
  UI_Label("Y:", (Rectangle){transformRow.x + 120, transformRow.y, 20, 20});
  UI_Spinner((Rectangle){transformRow.x + 140, transformRow.y, 80, 20},
             &state->transformY, 0, 1000);

  UI_Separator(UI_NextLayoutItem(&layout2, 0, 1));

  UI_Label("Components", UI_NextLayoutItem(&layout2, 0, 20));
  UI_Button("Add new components", UI_NextLayoutItem(&layout2, 0, 20),
            OnScriptOpenClicked, NULL);
}

void Gui_DrawBottomPanel(void) {
  EditorState *state = EditorState_Get();
  EditorLayout *layout = EditorLayout_Get();

  UI_Panel(layout->bottomPanel, "Project & Console");

  Rectangle content = layout->bottomPanel;
  content.y += 24;

  float tabW = 80;
  static int tab0 = 0, tab1 = 1;

  if (UI_Button("Assets", (Rectangle){content.x, content.y, tabW, 24}, NULL,
                NULL))
    state->activeTab = 0;
  if (UI_Button("Console", (Rectangle){content.x + tabW, content.y, tabW, 24},
                NULL, NULL))
    state->activeTab = 1;

  Rectangle view = {content.x + 5, content.y + 30, content.width - 10,
                    content.height - 60};
  UI_GroupBox(view, state->activeTab == 0 ? "File Browser" : "Log Output");

  if (state->activeTab == 0) {
    UI_Grid(view, 50, 3);
  } else {
    UI_Label("[INFO] Editor initialized successfully.",
             (Rectangle){view.x + 5, view.y + 5, view.width, 20});
    UI_Label("[WARN] No project loaded.",
             (Rectangle){view.x + 5, view.y + 25, view.width, 20});
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
  UICallback callback = NULL;

  static int indices[5];

  switch (state->activeMenuIndex) {
  case 0:
    items = fileItems;
    count = 5;
    callback = OnFileMenuClicked;
    break;
  case 1:
    items = editItems;
    count = 5;
    callback = OnEditMenuClicked;
    break;
  case 2:
    items = viewItems;
    count = 4;
    callback = OnViewMenuClicked;
    break;
  case 3:
    items = gameItems;
    count = 3;
    callback = OnGameMenuClicked;
    break;
  case 4:
    items = helpItems;
    count = 1;
    callback = OnHelpAbout;
    break;
  }

  if (items) {
    Rectangle bounds = {startX, startY, menuWidth, count * itemHeight + 2};
    UI_Panel(bounds, NULL);

    for (int i = 0; i < count; i++) {
      Rectangle itemBounds = {startX + 2, startY + 1 + i * itemHeight,
                              menuWidth - 4, itemHeight};

      if (strcmp(items[i], "---") == 0) {
        UI_Separator((Rectangle){itemBounds.x, itemBounds.y + itemHeight / 2,
                                 itemBounds.width, 1});
        continue;
      }

      indices[i] = i;
      if (UI_LabelButton(items[i], itemBounds, callback, &indices[i])) {
        state->activeMenuIndex = -1;
      }
    }
    DrawRectangleLinesEx(bounds, 1, Fade(BLACK, 0.5f));
  }
}

void Gui_ShowAlert(const char *title, const char *message,
                   const char *options) {
  UI_ShowAlert(title, message, options);
}

void Gui_DrawHierarchyContextMenu(void) {
  EditorState *state = EditorState_Get();

  if (!state->showHierarchyContextMenu)
    return;

  static UIContextMenu *menu = NULL;

  if (!menu) {
    menu = UI_CreateContextMenu("HierarchyContext",
                                state->hierarchyContextMenuPos);
    UI_AddMenuItem(menu, "Create Empty", OnHierarchyCreateEmpty, NULL);
    UI_AddMenuItem(menu, "Delete", OnHierarchyDelete, NULL);
    UI_AddMenuItem(menu, "Rename", OnHierarchyRename, NULL);
    UI_AddMenuItem(menu, "Duplicate", OnHierarchyDuplicate, NULL);
  } else {
    UI_SetContextMenuPosition(menu, state->hierarchyContextMenuPos);
  }

  if (!UI_ShowContextMenu(menu)) {
    state->showHierarchyContextMenu = false;
    menu = NULL;
  }
}

int Gui_DrawWarnMessageBox(void) { return UI_DrawMessageBoxes(); }
