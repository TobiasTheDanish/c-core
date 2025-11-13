#define RENDER_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#define INPUT_IMPLEMENTATION
#include "../gui.h"
#include "../input.h"
#include "../render.h"
#include "../types.h"

typedef struct {
  Bool quit;
  int32_t windowWidth;
  int32_t windowHeight;
} State;

void handleEvent(Event e, State *s) {
  switch (e.kind) {
  case EventKind_WindowResize:
    s->windowWidth = e.resize.width;
    s->windowHeight = e.resize.height;
    RendererWindowResize(e.resize.width, e.resize.height);
    return;
  case EventKind_Quit:
    s->quit = true;
    return;
  case EventKind_KeyDown:
  case EventKind_KeyUp:
  case EventKind_MouseMove:
    return;
  }
}

int main() {
  State state = {
      .windowWidth = 1080,
      .windowHeight = 720,
      .quit = 0,
  };
  CreateWindow(state.windowWidth, state.windowHeight);
  RendererWindowResize(state.windowWidth, state.windowHeight);

  UiContext *ctx = GUICreateContext();

  while (!state.quit) {
    Event *events = 0;
    int32_t count = 0;
    PollEvents(&events, &count);

    for (int i = 0; i < count; i++) {
      Event e = events[i];
      handleEvent(e, &state);
    }
    FreeMemory(events);

    ctx->maxSize[UiAxis_X] = state.windowWidth;
    ctx->maxSize[UiAxis_Y] = state.windowHeight;

    GUIBegin(ctx);

    UiWidget *row1 = GUI_RowBegin(ctx, StringFromCString("row1"));
    GUI_WidgetSize(*row1, UiAxis_X) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

    GUI_WidgetSize(*row1, UiAxis_Y) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1);

    GUI_ContainerBgColor(*row1) = NewColor(.bgra = 0xFFFF0000);
    GUI_RowEnd(ctx);

    UiWidget *row2 = GUI_RowBegin(ctx, StringFromCString("row2"));
    GUI_WidgetSize(*row2, UiAxis_X) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

    GUI_WidgetSize(*row2, UiAxis_Y) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 50, 1);

    GUI_ContainerBgColor(*row2) = NewColor(.bgra = 0xFF0000FF);
    {
      UiWidget *box1 = GUI_ColumnBegin(ctx, StringFromCString("box1"));
      GUI_WidgetSize(*box1, UiAxis_X) = GUI_UiSize(UiSizeKind_PIXELS, 40, 1);

      GUI_WidgetSize(*box1, UiAxis_Y) =
          GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

      GUI_ContainerBgColor(*box1) = NewColor(.bgra = 0xFFFF00FF);
      GUI_ColumnEnd(ctx);

      UiWidget *box2 = GUI_ColumnBegin(ctx, StringFromCString("box2"));
      GUI_WidgetSize(*box2, UiAxis_X) =
          GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 0);

      GUI_WidgetSize(*box2, UiAxis_Y) =
          GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

      GUI_ContainerBgColor(*box2) = NewColor(.bgra = 0xAAAAAAAA);
      GUI_ColumnEnd(ctx);

      UiWidget *box3 = GUI_ColumnBegin(ctx, StringFromCString("box3"));
      GUI_WidgetSize(*box3, UiAxis_X) = GUI_UiSize(UiSizeKind_PIXELS, 40, 1);

      GUI_WidgetSize(*box3, UiAxis_Y) =
          GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

      GUI_ContainerBgColor(*box3) = NewColor(.bgra = 0xFFFF00FF);
      GUI_ColumnEnd(ctx);
    }
    GUI_RowEnd(ctx);

    UiWidget *row3 = GUI_RowBegin(ctx, StringFromCString("row3"));
    GUI_WidgetSize(*row3, UiAxis_X) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

    GUI_WidgetSize(*row3, UiAxis_Y) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1);

    GUI_ContainerBgColor(*row3) = NewColor(.bgra = 0xFF00FF00);
    GUI_RowEnd(ctx);

    GUIEnd(ctx);

    RendererBlit();
  }

  return 0;
}
