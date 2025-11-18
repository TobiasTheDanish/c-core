#ifndef CORE_INPUT_H
#define CORE_INPUT_H
#include "common.h"

typedef enum {
  // EventKind_KeyDown events goes in the 'key' property
  EventKind_KeyDown,
  // EventKind_KeyUp events goes in the 'key' property
  EventKind_KeyUp,
  // EventKind_MouseMove events goes in the 'mouseMove' property
  EventKind_MouseMove,
  EventKind_WindowResize,
  EventKind_Quit,
} EventKind;

typedef enum {
  KeyCode_W = 25,
  KeyCode_A = 38,
  KeyCode_S,
  KeyCode_D,
  KeyCode_Space = 65,
  KeyCode_Count,
} KeyEvent_KeyCode;

typedef struct {
  KeyEvent_KeyCode keyCode;
} KeyEvent;

typedef struct {
  int32_t x;
  int32_t y;
} MouseMoveEvent;

typedef struct {
  int32_t width;
  int32_t height;
} WindowResizeEvent;

typedef struct {
  EventKind kind;
  union {
    KeyEvent key;
    MouseMoveEvent mouseMove;
    WindowResizeEvent resize;
  };
} Event;

void PollEvents(Event **_events, int32_t *count);

#ifdef INPUT_IMPLEMENTATION
#include "input/input.c"
#endif // INPUT_IMPLEMENTATION

#endif // !CORE_INPUT_H
