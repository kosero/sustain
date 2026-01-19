#ifndef SUSTAIN_UI_SYSTEM_H
#define SUSTAIN_UI_SYSTEM_H

#include <raylib.h>
#include <stdbool.h>

// Forward declarations
typedef struct UIContext UIContext;
typedef void (*UICallback)(void *userData);
typedef struct UIContextMenu UIContextMenu;
typedef struct UIPanel UIPanel;

void UI_Init(void);
void UI_Shutdown(void);

void UI_BeginFrame(void);
void UI_EndFrame(void);

typedef enum {
  UI_BUTTON_NORMAL,
  UI_BUTTON_TOGGLE,
  UI_BUTTON_ICON,
  UI_BUTTON_LABEL
} UIButtonType;

typedef struct {
  UIButtonType type;
  const char *text;
  Rectangle bounds;
  UICallback onClick;
  void *userData;
  bool *toggleState;
} UIButtonConfig;

bool UI_Button(const char *text, Rectangle bounds, UICallback onClick,
               void *userData);
bool UI_ToggleButton(const char *text, Rectangle bounds, bool *active);
bool UI_LabelButton(const char *text, Rectangle bounds, UICallback onClick,
                    void *userData);
bool UI_IconButton(const char *iconText, Rectangle bounds, UICallback onClick,
                   void *userData);

typedef struct UIMenuItem {
  const char *label;
  UICallback onSelect;
  void *userData;
  bool isSeparator;
  bool isDisabled;
} UIMenuItem;

UIContextMenu *UI_CreateContextMenu(const char *id, Vector2 position);
void UI_AddMenuItem(UIContextMenu *menu, const char *label, UICallback onSelect,
                    void *userData);
void UI_AddMenuSeparator(UIContextMenu *menu);
bool UI_ShowContextMenu(UIContextMenu *menu);
void UI_CloseContextMenu(UIContextMenu *menu);
void UI_SetContextMenuPosition(UIContextMenu *menu, Vector2 position);
bool UI_IsContextMenuOpen(void);

typedef struct {
  const char *title;
  Rectangle bounds;
  bool hasTitle;
} UIPanelConfig;
void UI_BeginPanel(const char *title, Rectangle bounds);
void UI_EndPanel(void);
void UI_Panel(Rectangle bounds, const char *title);

void UI_Label(const char *text, Rectangle bounds);
bool UI_TextBox(Rectangle bounds, char *buffer, int bufferSize, bool *editMode);

bool UI_Checkbox(Rectangle bounds, const char *label, bool *checked);
bool UI_Spinner(Rectangle bounds, int *value, int minValue, int maxValue);
bool UI_Slider(Rectangle bounds, float *value, float minValue, float maxValue);
bool UI_Dropdown(Rectangle bounds, const char *text, int *active,
                 bool editMode);

typedef struct {
  Rectangle bounds;
  const char **items;
  int itemCount;
  int *scrollIndex;
  int *activeItem;
} UIListViewConfig;

bool UI_ListView(Rectangle bounds, const char *items, int *scrollIndex,
                 int *activeItem);

typedef struct {
  float x, y;
  float width, height;
  float spacing;
  float padding;
  int itemsPerRow;
  int currentItem;
} UILayout;

UILayout UI_CreateVerticalLayout(Rectangle bounds, float spacing,
                                 float padding);
UILayout UI_CreateHorizontalLayout(Rectangle bounds, float spacing,
                                   float padding);
UILayout UI_CreateGridLayout(Rectangle bounds, int columns, float spacing,
                             float padding);
Rectangle UI_NextLayoutItem(UILayout *layout, float itemWidth,
                            float itemHeight);
void UI_LayoutNextRow(UILayout *layout);

void UI_Separator(Rectangle bounds);
void UI_GroupBox(Rectangle bounds, const char *text);
void UI_Grid(Rectangle bounds, float spacing, int subdivs);
bool UI_IsHovered(Rectangle bounds);
bool UI_IsClicked(Rectangle bounds);
bool UI_IsRightClicked(Rectangle bounds);
Rectangle UI_GetPanelContent(Rectangle panelBounds);

typedef enum {
  UI_MSGBOX_INFO,
  UI_MSGBOX_WARNING,
  UI_MSGBOX_ERROR,
  UI_MSGBOX_QUESTION
} UIMessageBoxType;

int UI_MessageBox(Rectangle bounds, const char *title, const char *message,
                  const char *buttons);
void UI_ShowAlert(const char *title, const char *message,
                  const char *buttonText);
int UI_DrawMessageBoxes(void);

#endif // SUSTAIN_UI_SYSTEM_H
