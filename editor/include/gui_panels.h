#ifndef GUI_PANELS_H
#define GUI_PANELS_H

void Gui_Init(void);
void Gui_DrawMenuBar(void);
void Gui_DrawToolbar(void);
void Gui_DrawHierarchy(void);
void Gui_DrawInspector(void);
void Gui_DrawBottomPanel(void);
void Gui_DrawSceneView(void);
void Gui_DrawDropdowns(void);
void Gui_DrawHierarchyContextMenu(void);
void Gui_ShowAlert(const char *title, const char *message, const char *options);
int Gui_DrawWarnMessageBox(void);
int Gui_DrawAboutDialog(void);

#endif
