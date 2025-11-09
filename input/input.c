#include "../input.h"

#define ALLOCATOR_IMPLEMENTATION
#include "../allocator.h"

void __AppendEvent(Event **events, int *count, Event e) {
  if (events == 0) {
    return;
  }

  if (*events == 0) {
    *events = AllocMemory(sizeof(Event));
    (*events)[0] = e;
    *count = 1;
  } else {
    *events = ReallocMemory(*events, ((*count) + 1) * sizeof(Event));
    (*events)[*count] = e;
    *count += 1;
  }
}

#ifdef __linux__
#define RENDER_IMPLEMENTATION
#include "../render.h"

void PollEvents(Event **_events, int32_t *count) {
  int eventsPending = XPending(__display);
  Event *events = NULL;
  int eventCount = 0;

  XEvent xEvent;
  for (int i = 0; i < eventsPending; i++) {
    XNextEvent(__display, &xEvent);
    Event e;
    if (xEvent.type == ConfigureNotify) {
      uint32_t width = xEvent.xconfigure.width;
      uint32_t height = xEvent.xconfigure.height;
      e = (Event){
          .kind = EventKind_WindowResize,
          .resize =
              {
                  .width = width,
                  .height = height,
              },
      };
    } else if (xEvent.type == KeyPress) {
      uint32_t key = xEvent.xkey.keycode;
      e = (Event){
          .kind = EventKind_KeyDown,
          .key =
              {
                  .keyCode = key,
              },
      };
    } else if (xEvent.type == KeyRelease) {
      uint32_t key = xEvent.xkey.keycode;
      e = (Event){
          .kind = EventKind_KeyUp,
          .key =
              {
                  .keyCode = key,
              },
      };
    } else if (xEvent.type == MotionNotify &&
               xEvent.xmotion.is_hint == NotifyNormal) {
      int32_t x = xEvent.xmotion.x;
      int32_t y = xEvent.xmotion.y;
      e = (Event){
          .kind = EventKind_MouseMove,
          .mouseMove =
              {
                  .x = x,
                  .y = y,
              },
      };
    } else if (xEvent.type == ClientMessage) {
      if ((Atom)xEvent.xclient.data.l[0] == __wm_delete_window) {
        e = (Event){
            .kind = EventKind_Quit,
        };
      }
    }

    __AppendEvent(&events, &eventCount, e);
  }

  *_events = events;
  *count = eventCount;
}

#endif
