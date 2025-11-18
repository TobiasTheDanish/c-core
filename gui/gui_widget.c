#include "../gui.h"

/** The String passed through 'name' param is freed in this function */
UiWidget *GUI_RowBegin(UiContext *ctx, String name) {
  UiWidget *w = UiWidgetFromString(ctx, name);

  PushParentWidget(ctx, w);

  return w;
}

UiSignal GUI_RowEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  return UiSignalFromWidget(ctx, w);
}

/** The String passed through 'name' param is freed in this function */
UiWidget *GUI_ColumnBegin(UiContext *ctx, String name) {
  UiWidget *w = UiWidgetFromString(ctx, name);

  PushParentWidget(ctx, w);

  return w;
}

UiSignal GUI_ColumnEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  return UiSignalFromWidget(ctx, w);
}
