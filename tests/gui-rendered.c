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
  Bool showText;
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
      .showText = 0,
  };
  OS_Window *w = OS_CreateWindow(state.windowWidth, state.windowHeight);
  OS_LoadFont("/home/tobias/dev/c-core/fonts/UniversCondensed.ttf", 48);

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

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 0))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1)) {
        GUI_Spacer(ctx, StringFromCString("row2spacer"));
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
                                                  : 0xFF00FF00))
                GUI_Row(ctx, StringFromCString("row3")) {

      GUI_Width(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 0))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1))
              GUI_BgColor(ctx, NewColor(.bgra = 0xFFFFFFFF))
                  GUI_ContentColor(ctx, NewColor(.bgra = 0xFF181818))
                      GUI_Row(ctx, StringFromCString("row3_text_container")) {
        String text = StringFromCString("Hello world");
        if (state.showText) {
          CORE_Dimensions textDim = OS_TextDimensions(text, StringLength(text));
          GUI_Width(ctx, GUI_UiSize(UiSizeKind_TEXT, textDim.width, 1))
              GUI_Height(ctx, GUI_UiSize(UiSizeKind_TEXT, textDim.height, 1))
                  GUI_BgColor(ctx, NewColor(.bgra = 0xFF040506))
                      GUI_ContentColor(ctx, NewColor(.bgra = 0xFFF8F8F8))
                          GUI_Text(ctx, text);
        }
      }

      String buttonText = StringFromCString("Press me");
      CORE_Dimensions textDim =
          OS_TextDimensions(buttonText, StringLength(buttonText));
      GUI_Width(ctx, GUI_UiSize(UiSizeKind_TEXT, textDim.width, 1))
          GUI_Height(ctx, GUI_UiSize(UiSizeKind_TEXT, textDim.height, 1))
              GUI_BgColor(ctx, NewColor(.bgra = 0xFFAAAAAA))
                  GUI_ContentColor(ctx, NewColor(.bgra = 0xFF181818)) {

        UiSignal s = GUI_Button(ctx, buttonText);
        if (GUI_Hovering(s)) {
          s.w->data.bg = NewColor(.bgra = 0xFFDDDDDD);
        }
        if (GUI_Clicked(s)) {
          s.w->data.bg = NewColor(.bgra = 0xFFBDBDBD);
          state.showText = !state.showText;

          printf("button clicked. showText: %d\n", state.showText);
        }
      }
    }

    GUIEnd(ctx, w);

    OS_BlitWindow(w);
  }

  return 0;
}
