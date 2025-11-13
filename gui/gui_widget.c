#include "../gui.h"

/** The String passed through 'name' param is freed in this function */
UiWidget *GUI_RowBegin(UiContext *ctx, String name) {
  UiWidget *w = UiWidgetFromString(ctx, name);

  w->data = GUI_ContainerData(UiAxis_X);

  PushParentWidget(ctx, w);

  return w;
}

void GUI_RowEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  (void)w; // Test if w is hot or active
}

/** The String passed through 'name' param is freed in this function */
UiWidget *GUI_ColumnBegin(UiContext *ctx, String name) {
  UiWidget *w = UiWidgetFromString(ctx, name);

  w->data = GUI_ContainerData(UiAxis_Y);

  PushParentWidget(ctx, w);

  return w;
}

void GUI_ColumnEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  (void)w; // Test if w is hot or active
}
