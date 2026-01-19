#include "sustain/ui/ui_system.h"
#define RAYGUI_IMPLEMENTATION
#include "sustain/ui/raygui.h"
#include <stdlib.h>
#include <string.h>

#define MAX_CONTEXT_MENUS 8
#define MAX_MENU_ITEMS 32
#define MAX_PANELS 16

typedef struct {
  UIMenuItem items[MAX_MENU_ITEMS];
  int itemCount;
  Vector2 position;
  Rectangle bounds;
  bool isActive;
  char id[64];
} ContextMenuState;

typedef struct {
  Rectangle bounds;
  char title[128];
  bool hasTitle;
  Rectangle contentArea;
} PanelState;

typedef struct {
  bool initialized;

  ContextMenuState contextMenus[MAX_CONTEXT_MENUS];
  int contextMenuCount;

  PanelState panelStack[MAX_PANELS];
  int panelStackTop;

  bool showMessageBox;
  char msgBoxTitle[64];
  char msgBoxMessage[256];
  char msgBoxButtons[64];
  Rectangle msgBoxBounds;

  Vector2 mousePos;
  bool mousePressed;
  bool mouseRightPressed;
} UISystemState;

static UISystemState uiState = {0};

void UI_Init(void) {
  if (uiState.initialized) {
    return;
  }

  memset(&uiState, 0, sizeof(UISystemState));
  uiState.initialized = true;

  GuiLoadStyleDark();
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
  GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
}

void UI_Shutdown(void) {
  if (!uiState.initialized) {
    return;
  }

  for (int i = 0; i < uiState.contextMenuCount; i++) {
    uiState.contextMenus[i].isActive = false;
  }

  uiState.initialized = false;
}

void UI_BeginFrame(void) {
  if (!uiState.initialized) {
    return;
  }

  uiState.mousePos = GetMousePosition();
  uiState.mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  uiState.mouseRightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
  uiState.panelStackTop = 0;
}

void UI_EndFrame(void) {
  if (!uiState.initialized) {
    return;
  }

  for (int i = uiState.contextMenuCount - 1; i >= 0; i--) {
    if (!uiState.contextMenus[i].isActive) {
      for (int j = i; j < uiState.contextMenuCount - 1; j++) {
        uiState.contextMenus[j] = uiState.contextMenus[j + 1];
      }
      uiState.contextMenuCount--;
    }
  }
}

bool UI_Button(const char *text, Rectangle bounds, UICallback onClick,
               void *userData) {
  bool clicked = GuiButton(bounds, text);
  if (clicked && onClick) {
    onClick(userData);
  }
  return clicked;
}

bool UI_ToggleButton(const char *text, Rectangle bounds, bool *active) {
  return GuiToggle(bounds, text, active);
}

bool UI_LabelButton(const char *text, Rectangle bounds, UICallback onClick,
                    void *userData) {
  bool clicked = GuiLabelButton(bounds, text);
  if (clicked && onClick) {
    onClick(userData);
  }
  return clicked;
}

bool UI_IconButton(const char *iconText, Rectangle bounds, UICallback onClick,
                   void *userData) {
  bool clicked = GuiButton(bounds, iconText);
  if (clicked && onClick) {
    onClick(userData);
  }
  return clicked;
}

UIContextMenu *UI_CreateContextMenu(const char *id, Vector2 position) {
  if (uiState.contextMenuCount >= MAX_CONTEXT_MENUS) {
    return NULL;
  }

  ContextMenuState *menu = &uiState.contextMenus[uiState.contextMenuCount];
  memset(menu, 0, sizeof(ContextMenuState));

  strncpy(menu->id, id, 63);
  menu->id[63] = '\0';
  menu->position = position;
  menu->isActive = true;
  menu->itemCount = 0;

  uiState.contextMenuCount++;

  return (UIContextMenu *)menu;
}

void UI_AddMenuItem(UIContextMenu *menuHandle, const char *label,
                    UICallback onSelect, void *userData) {
  if (!menuHandle)
    return;

  ContextMenuState *menu = (ContextMenuState *)menuHandle;
  if (menu->itemCount >= MAX_MENU_ITEMS) {
    return;
  }

  UIMenuItem *item = &menu->items[menu->itemCount];
  item->label = label;
  item->onSelect = onSelect;
  item->userData = userData;
  item->isSeparator = false;
  item->isDisabled = false;

  menu->itemCount++;
}

void UI_AddMenuSeparator(UIContextMenu *menuHandle) {
  if (!menuHandle)
    return;

  ContextMenuState *menu = (ContextMenuState *)menuHandle;
  if (menu->itemCount >= MAX_MENU_ITEMS) {
    return;
  }

  UIMenuItem *item = &menu->items[menu->itemCount];
  item->label = "---";
  item->onSelect = NULL;
  item->userData = NULL;
  item->isSeparator = true;
  item->isDisabled = false;

  menu->itemCount++;
}

bool UI_ShowContextMenu(UIContextMenu *menuHandle) {
  if (!menuHandle)
    return false;

  ContextMenuState *menu = (ContextMenuState *)menuHandle;
  if (!menu->isActive) {
    return false;
  }

  float menuWidth = 140;
  float itemHeight = 24;
  float menuHeight = menu->itemCount * itemHeight + 4;

  menu->bounds =
      (Rectangle){menu->position.x, menu->position.y, menuWidth, menuHeight};

  GuiPanel(menu->bounds, NULL);

  for (int i = 0; i < menu->itemCount; i++) {
    UIMenuItem *item = &menu->items[i];
    Rectangle itemBounds = {menu->bounds.x + 2,
                            menu->bounds.y + 2 + i * itemHeight, menuWidth - 4,
                            itemHeight};

    if (item->isSeparator) {
      GuiLine((Rectangle){itemBounds.x, itemBounds.y + itemHeight / 2,
                          itemBounds.width, 1},
              NULL);
      continue;
    }

    if (GuiLabelButton(itemBounds, item->label)) {
      if (item->onSelect) {
        item->onSelect(item->userData);
      }
      menu->isActive = false;
    }
  }

  DrawRectangleLinesEx(menu->bounds, 1, Fade(BLACK, 0.5f));

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
      IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    Vector2 mousePos = GetMousePosition();
    if (!CheckCollisionPointRec(mousePos, menu->bounds)) {
      menu->isActive = false;
    }
  }

  return menu->isActive;
}

void UI_CloseContextMenu(UIContextMenu *menuHandle) {
  if (!menuHandle)
    return;

  ContextMenuState *menu = (ContextMenuState *)menuHandle;
  menu->isActive = false;
}

void UI_SetContextMenuPosition(UIContextMenu *menuHandle, Vector2 position) {
  if (!menuHandle)
    return;

  ContextMenuState *menu = (ContextMenuState *)menuHandle;
  menu->position = position;
}

bool UI_IsContextMenuOpen(void) {
  for (int i = 0; i < uiState.contextMenuCount; i++) {
    if (uiState.contextMenus[i].isActive) {
      return true;
    }
  }
  return false;
}

void UI_BeginPanel(const char *title, Rectangle bounds) {
  if (uiState.panelStackTop >= MAX_PANELS) {
    return;
  }

  PanelState *panel = &uiState.panelStack[uiState.panelStackTop];
  panel->bounds = bounds;
  strncpy(panel->title, title ? title : "", 127);
  panel->title[127] = '\0';
  panel->hasTitle = (title != NULL && title[0] != '\0');

  panel->contentArea = bounds;
  if (panel->hasTitle) {
    panel->contentArea.y += 24;
    panel->contentArea.height -= 24;
  }

  GuiPanel(bounds, panel->hasTitle ? panel->title : NULL);

  uiState.panelStackTop++;
}

void UI_EndPanel(void) {
  if (uiState.panelStackTop > 0) {
    uiState.panelStackTop--;
  }
}

void UI_Panel(Rectangle bounds, const char *title) { GuiPanel(bounds, title); }

void UI_Label(const char *text, Rectangle bounds) { GuiLabel(bounds, text); }

bool UI_TextBox(Rectangle bounds, char *buffer, int bufferSize,
                bool *editMode) {
  return GuiTextBox(bounds, buffer, bufferSize, *editMode);
}

bool UI_Checkbox(Rectangle bounds, const char *label, bool *checked) {
  return GuiCheckBox(bounds, label, checked);
}

bool UI_Spinner(Rectangle bounds, int *value, int minValue, int maxValue) {
  return GuiSpinner(bounds, NULL, value, minValue, maxValue, false);
}

bool UI_Slider(Rectangle bounds, float *value, float minValue, float maxValue) {
  return GuiSlider(bounds, NULL, NULL, value, minValue, maxValue);
}

bool UI_Dropdown(Rectangle bounds, const char *text, int *active,
                 bool editMode) {
  return GuiDropdownBox(bounds, text, active, editMode);
}

bool UI_ListView(Rectangle bounds, const char *items, int *scrollIndex,
                 int *activeItem) {
  int oldActive = *activeItem;
  GuiListView(bounds, items, scrollIndex, activeItem);
  return (*activeItem != oldActive);
}

UILayout UI_CreateVerticalLayout(Rectangle bounds, float spacing,
                                 float padding) {
  UILayout layout = {0};
  layout.x = bounds.x + padding;
  layout.y = bounds.y + padding;
  layout.width = bounds.width - padding * 2;
  layout.height = bounds.height - padding * 2;
  layout.spacing = spacing;
  layout.padding = padding;
  layout.itemsPerRow = 1;
  layout.currentItem = 0;
  return layout;
}

UILayout UI_CreateHorizontalLayout(Rectangle bounds, float spacing,
                                   float padding) {
  UILayout layout = {0};
  layout.x = bounds.x + padding;
  layout.y = bounds.y + padding;
  layout.width = bounds.width - padding * 2;
  layout.height = bounds.height - padding * 2;
  layout.spacing = spacing;
  layout.padding = padding;
  layout.itemsPerRow = -1;
  layout.currentItem = 0;
  return layout;
}

UILayout UI_CreateGridLayout(Rectangle bounds, int columns, float spacing,
                             float padding) {
  UILayout layout = {0};
  layout.x = bounds.x + padding;
  layout.y = bounds.y + padding;
  layout.width = bounds.width - padding * 2;
  layout.height = bounds.height - padding * 2;
  layout.spacing = spacing;
  layout.padding = padding;
  layout.itemsPerRow = columns;
  layout.currentItem = 0;
  return layout;
}

Rectangle UI_NextLayoutItem(UILayout *layout, float itemWidth,
                            float itemHeight) {
  Rectangle result = {0};

  if (layout->itemsPerRow == 1) {
    // Vertical layout
    result.x = layout->x;
    result.y = layout->y + layout->currentItem * (itemHeight + layout->spacing);
    result.width = itemWidth > 0 ? itemWidth : layout->width;
    result.height = itemHeight;
    layout->currentItem++;
  } else if (layout->itemsPerRow == -1) {
    // Horizontal layout
    result.x = layout->x + layout->currentItem * (itemWidth + layout->spacing);
    result.y = layout->y;
    result.width = itemWidth;
    result.height = itemHeight > 0 ? itemHeight : layout->height;
    layout->currentItem++;
  } else {
    // Grid layout
    int row = layout->currentItem / layout->itemsPerRow;
    int col = layout->currentItem % layout->itemsPerRow;
    result.x = layout->x + col * (itemWidth + layout->spacing);
    result.y = layout->y + row * (itemHeight + layout->spacing);
    result.width = itemWidth;
    result.height = itemHeight;
    layout->currentItem++;
  }

  return result;
}

void UI_LayoutNextRow(UILayout *layout) {
  if (layout->itemsPerRow > 0) {
    int row = (layout->currentItem / layout->itemsPerRow) + 1;
    layout->currentItem = row * layout->itemsPerRow;
  }
}

void UI_Separator(Rectangle bounds) { GuiLine(bounds, NULL); }

void UI_GroupBox(Rectangle bounds, const char *text) {
  GuiGroupBox(bounds, text);
}

void UI_Grid(Rectangle bounds, float spacing, int subdivs) {
  GuiGrid(bounds, NULL, spacing, subdivs, NULL);
}

bool UI_IsHovered(Rectangle bounds) {
  return CheckCollisionPointRec(GetMousePosition(), bounds);
}

bool UI_IsClicked(Rectangle bounds) {
  return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && UI_IsHovered(bounds);
}

bool UI_IsRightClicked(Rectangle bounds) {
  return IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && UI_IsHovered(bounds);
}

Rectangle UI_GetPanelContent(Rectangle panelBounds) {
  Rectangle content = panelBounds;
  content.y += 24;
  content.height -= 24;
  return content;
}

int UI_MessageBox(Rectangle bounds, const char *title, const char *message,
                  const char *buttons) {
  return GuiMessageBox(bounds, title, message, buttons);
}

void UI_ShowAlert(const char *title, const char *message,
                  const char *buttonText) {
  strncpy(uiState.msgBoxTitle, title, 63);
  uiState.msgBoxTitle[63] = '\0';

  strncpy(uiState.msgBoxMessage, message, 255);
  uiState.msgBoxMessage[255] = '\0';

  strncpy(uiState.msgBoxButtons, buttonText, 63);
  uiState.msgBoxButtons[63] = '\0';

  uiState.showMessageBox = true;
}

int UI_DrawMessageBoxes(void) {
  if (!uiState.showMessageBox) {
    return -1;
  }

  Rectangle bounds = {GetScreenWidth() / 2.0f - 200,
                      GetScreenHeight() / 2.0f - 100, 400, 200};

  int result = GuiMessageBox(bounds, uiState.msgBoxTitle, uiState.msgBoxMessage,
                             uiState.msgBoxButtons);

  if (result >= 0) {
    uiState.showMessageBox = false;
  }

  return result;
}
