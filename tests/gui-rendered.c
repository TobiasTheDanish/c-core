#define GUI_IMPLEMENTATION
#define CORE_OS_IMPLEMENTATION
#include "../common.h"
#include "../gui.h"
#include "../os.h"

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

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1))
            GUI_BgColor(ctx, NewColor(.bgra = 0xFFFF0000)) {
      UiWidget *row1 = GUI_RowBegin(ctx, StringFromCString("row1"));
      if (GUI_Hovering(GUI_RowEnd(ctx))) {
        row1->data.bg = NewColor(.bgra = 0xFFAA0000);
      }
    }

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 50, 1))
            GUI_BgColor(ctx, NewColor(.bgra = 0xFF0000FF))
                GUI_Row(ctx, StringFromCString("row2")) {

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 15, 1))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
              GUI_BgColor(ctx, NewColor(.bgra = 0xFFFF00FF)) {
        UiWidget *box1 = GUI_ColumnBegin(ctx, StringFromCString("box1"));
        if (GUI_Hovering(GUI_ColumnEnd(ctx))) {
          box1->data.bg = NewColor(.bgra = 0xFFAA00AA);
        }
      }

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 0))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
              GUI_BgColor(ctx, NewColor(.bgra = 0xAAAAAAAA)) {
        UiWidget *box2 = GUI_ColumnBegin(ctx, StringFromCString("box2"));
        if (GUI_Hovering(GUI_ColumnEnd(ctx))) {
          box2->data.bg = NewColor(.bgra = 0x55555555);
        }
      }

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 15, 1))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
              GUI_BgColor(ctx, NewColor(.bgra = 0xFFFF00FF)) {
        UiWidget *box3 = GUI_ColumnBegin(ctx, StringFromCString("box3"));
        if (GUI_Hovering(GUI_ColumnEnd(ctx))) {
          box3->data.bg = NewColor(.bgra = 0xFFAA00AA);
        }
      }
    }

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1))
            GUI_BgColor(ctx, NewColor(.bgra = 0xFF00FF00)) {
      UiWidget *row3 = GUI_RowBegin(ctx, StringFromCString("row3"));
      if (GUI_Hovering(GUI_RowEnd(ctx))) {
        row3->data.bg = NewColor(.bgra = 0xFF00AA00);
      }
    }

    GUIEnd(ctx, w);

    OS_BlitWindow(w);
  }

  return 0;
}
