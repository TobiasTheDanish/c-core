#include <stdio.h>
#define CORE_IMPLEMENTATION
#include "../core.h"

enum {
  ElementRow1 = 1,
  ElementRow2,
  ElementRow3,
  ElementBox1,
  ElementBox2,
  ElementBox3,
};

typedef struct {
  Bool quit;
  int32_t windowWidth;
  int32_t windowHeight;
  int32_t mouseX;
  int32_t mouseY;

  uint8_t activeElement;
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

int32_t activeColor = 0xFF55AAFF;

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

    GUI_HandleEvents(ctx, events, count);
    for (int i = 0; i < count; i++) {
      OS_Event e = events[i];
      handleEvent(e, &state);
    }
    FreeMemory(events);

    // GUI_SetMousePos(ctx, state.mouseX, state.mouseY);
    // ctx->maxSize[UiAxis_X] = state.windowWidth;
    // ctx->maxSize[UiAxis_Y] = state.windowHeight;

    GUIBegin(ctx);

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1))
            GUI_BgColor(ctx, NewColor(.bgra = state.activeElement == ElementRow1
                                                  ? activeColor
                                                  : 0xFFFF0000)) {
      UiSignal s = GUI_Box(ctx, StringFromCString("row1"));
      if (GUI_Hovering(s)) {
        s.w->data.bg = NewColor(.bgra = 0xFFAA0000);
      }
      if (GUI_Clicked(s)) {
        printf("row1 clicked\n");
        state.activeElement = ElementRow1;
      }
    }

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 50, 1))
            GUI_Row(ctx, StringFromCString("row2")) {

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 15, 1)) GUI_Height(
          ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
          GUI_BgColor(ctx, NewColor(.bgra = state.activeElement == ElementBox1
                                                ? activeColor
                                                : 0xFFFF00FF)) {
        UiSignal s = GUI_Box(ctx, StringFromCString("box1"));
        if (GUI_Hovering(s)) {
          s.w->data.bg = NewColor(.bgra = 0xFFFF00AA);
        }
        if (GUI_Clicked(s)) {
          printf("box1 clicked\n");
          state.activeElement = ElementBox1;
        }
      }

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 0)) GUI_Height(
          ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
          GUI_BgColor(ctx, NewColor(.bgra = state.activeElement == ElementBox2
                                                ? activeColor
                                                : 0xFFAAAAAA)) {
        UiSignal s = GUI_Box(ctx, StringFromCString("box2"));
        if (GUI_Hovering(s)) {
          s.w->data.bg = NewColor(.bgra = 0xFF555555);
        }
        if (GUI_Clicked(s)) {
          printf("box2 clicked\n");
          state.activeElement = ElementBox2;
        }
      }

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 15, 1)) GUI_Height(
          ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
          GUI_BgColor(ctx, NewColor(.bgra = state.activeElement == ElementBox3
                                                ? activeColor
                                                : 0xFFFF00FF)) {
        UiSignal s = GUI_Box(ctx, StringFromCString("box3"));
        if (GUI_Hovering(s)) {
          s.w->data.bg = NewColor(.bgra = 0xFFFF00AA);
        }
        if (GUI_Clicked(s)) {
          printf("box3 clicked\n");
          state.activeElement = ElementBox3;
        }
      }
    }

    GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
        GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 25, 1))
            GUI_BgColor(ctx, NewColor(.bgra = state.activeElement == ElementRow3
                                                  ? activeColor
                                                  : 0xFF00FF00)) {
      UiSignal s = GUI_Box(ctx, StringFromCString("row3"));
      if (GUI_Hovering(s)) {
        s.w->data.bg = NewColor(.bgra = 0xFF00AA00);
      }
      if (GUI_Clicked(s)) {
        printf("row3 clicked\n");
        state.activeElement = ElementRow3;
      }
    }

    GUIEnd(ctx, w);

    OS_BlitWindow(w);
  }

  return 0;
}
