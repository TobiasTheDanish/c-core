#define GUI_IMPLEMENTATION
#define CORE_OS_IMPLEMENTATION
#include "../gui.h"
#include "../os.h"
#include "../types.h"

typedef struct {
  Bool quit;
  int32_t windowWidth;
  int32_t windowHeight;
  int32_t mouseX;
  int32_t mouseY;
} State;

void handleEvent(OS_Event e, State *s) {
  switch (e.kind) {
  case OS_EventKind_WindowResize:
    s->windowWidth = e.resize.width;
    s->windowHeight = e.resize.height;
    return;
  case OS_EventKind_Quit: {
    s->quit = true;
    return;
  }
  case OS_EventKind_MouseMove: {
    s->mouseX = e.mouseMove.x;
    s->mouseY = e.mouseMove.y;
    return;
  }
  case OS_EventKind_KeyDown:
  case OS_EventKind_KeyUp:
    return;
  }
}

int main() {
  State state = {
      .windowWidth = 1080,
      .windowHeight = 720,
      .quit = 0,
  };
  OS_Window *w = OS_CreateWindow(state.windowWidth, state.windowHeight);

  UiContext *ctx = GUICreateContext();

  while (!state.quit) {
    OS_Event *events = 0;
    int32_t count = 0;
    OS_PollEvents(w, &events, &count);

    for (int i = 0; i < count; i++) {
      OS_Event e = events[i];
      handleEvent(e, &state);
    }
    FreeMemory(events);

    GUI_SetMousePos(ctx, state.mouseX, state.mouseY);
    ctx->maxSize[UiAxis_X] = state.windowWidth;
    ctx->maxSize[UiAxis_Y] = state.windowHeight;

    GUIBegin(ctx);

    UiWidget *row1 = GUI_RowBegin(ctx, StringFromCString("row1"));
    GUI_WidgetSize(*row1, UiAxis_X) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

    GUI_WidgetSize(*row1, UiAxis_Y) =
        GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1);

    GUI_ContainerBgColor(*row1) = NewColor(.bgra = 0xFFFF0000);
    if (GUI_Hovering(GUI_RowEnd(ctx))) {
      GUI_ContainerBgColor(*row1) = NewColor(.bgra = 0xFFAA0000);
    }

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
    if (GUI_Hovering(GUI_RowEnd(ctx))) {
      GUI_ContainerBgColor(*row3) = NewColor(.bgra = 0xFF00AA00);
    }

    GUIEnd(ctx, w);

    OS_BlitWindow(w);
  }

  return 0;
}
