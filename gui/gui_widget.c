#include "../gui.h"

UiWidget *GUI_RowBegin(UiContext *ctx, String name) {
  PushLayoutAxis(ctx, UiAxis_X);
  UiWidget *w = UiWidgetFromString(ctx, name, UiWidgetFlag_None);
  PopLayoutAxis(ctx);

  PushParentWidget(ctx, w);

  return w;
}

UiSignal GUI_RowEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  return UiSignalFromWidget(ctx, w);
}

UiWidget *GUI_ColumnBegin(UiContext *ctx, String name) {
  PushLayoutAxis(ctx, UiAxis_Y);
  UiWidget *w = UiWidgetFromString(ctx, name, UiWidgetFlag_None);
  PopLayoutAxis(ctx);

  PushParentWidget(ctx, w);

  return w;
}

UiSignal GUI_ColumnEnd(UiContext *ctx) {
  UiWidget *w = PopParentWidget(ctx);
  return UiSignalFromWidget(ctx, w);
}

UiSignal GUI_Box(UiContext *ctx, String name) {
  UiWidget *w = UiWidgetFromString(ctx, name, UiWidgetFlag_None);
  return UiSignalFromWidget(ctx, w);
}

void GUI_Spacer(UiContext *ctx, String key) {
  UiWidgetFromString(ctx, key, UiWidgetFlag_NoRender);
}

UiSignal GUI_Button(UiContext *ctx, String text) {
  UiWidget *w = UiWidgetFromString(ctx, text, UiWidgetFlag_RenderText);
  return UiSignalFromWidget(ctx, w);
}

UiSignal GUI_Text(UiContext *ctx, String text) {
  UiWidget *w = UiWidgetFromString(ctx, text, UiWidgetFlag_RenderText);
  return UiSignalFromWidget(ctx, w);
}
