#ifndef CORE_OS_H
#define CORE_OS_H
#include "common.h"

typedef struct OS_WINDOW OS_Window;
typedef enum {
  /** EventKind_KeyDown events goes in the 'key' property */
  OS_EventKind_KeyDown,
  /** EventKind_KeyUp events goes in the 'key' property */
  OS_EventKind_KeyUp,
  /** EventKind_MouseMove events goes in the 'mouseMove' property */
  OS_EventKind_MouseMove,
  OS_EventKind_WindowResize,
  OS_EventKind_Quit,
} OS_EventKind;

typedef enum {
  OS_KeyCode_keyNULL = 0,
  OS_KeyCode_escape = '\033',
  OS_KeyCode_backtick = '`',
  OS_KeyCode_0 = '0',
  OS_KeyCode_1 = '1',
  OS_KeyCode_2 = '2',
  OS_KeyCode_3 = '3',
  OS_KeyCode_4 = '4',
  OS_KeyCode_5 = '5',
  OS_KeyCode_6 = '6',
  OS_KeyCode_7 = '7',
  OS_KeyCode_8 = '8',
  OS_KeyCode_9 = '9',
  OS_KeyCode_minus = '-',
  OS_KeyCode_equals = '=',
  OS_KeyCode_backSpace = '\b',
  OS_KeyCode_tab = '\t',
  OS_KeyCode_space = ' ',
  OS_KeyCode_a = 'a',
  OS_KeyCode_b = 'b',
  OS_KeyCode_c = 'c',
  OS_KeyCode_d = 'd',
  OS_KeyCode_e = 'e',
  OS_KeyCode_f = 'f',
  OS_KeyCode_g = 'g',
  OS_KeyCode_h = 'h',
  OS_KeyCode_i = 'i',
  OS_KeyCode_j = 'j',
  OS_KeyCode_k = 'k',
  OS_KeyCode_l = 'l',
  OS_KeyCode_m = 'm',
  OS_KeyCode_n = 'n',
  OS_KeyCode_o = 'o',
  OS_KeyCode_p = 'p',
  OS_KeyCode_q = 'q',
  OS_KeyCode_r = 'r',
  OS_KeyCode_s = 's',
  OS_KeyCode_t = 't',
  OS_KeyCode_u = 'u',
  OS_KeyCode_v = 'v',
  OS_KeyCode_w = 'w',
  OS_KeyCode_x = 'x',
  OS_KeyCode_y = 'y',
  OS_KeyCode_z = 'z',
  OS_KeyCode_period = '.',
  OS_KeyCode_comma = ',',
  OS_KeyCode_slash = '/',
  OS_KeyCode_bracket = '{',
  OS_KeyCode_closeBracket = '}',
  OS_KeyCode_semicolon = ';',
  OS_KeyCode_apostrophe = '\'',
  OS_KeyCode_backSlash = '\\',
  OS_KeyCode_return = '\n',
  OS_KeyCode_delete = '\177', /* 127 */
  OS_KeyCode_mouseLeft = 128,
  OS_KeyCode_mouseMiddle = 129,
  OS_KeyCode_mouseRight = 130,
  OS_KeyCode_mouseScrollUp = 131,
  OS_KeyCode_mouseScrollDown = 132,
  OS_KeyCode_count = 256,
} OS_KeyEvent_KeyCode;

typedef struct {
  OS_KeyEvent_KeyCode keyCode;
} OS_KeyEvent;

typedef struct {
  int32_t x;
  int32_t y;
} OS_MouseMoveEvent;

typedef struct {
  int32_t width;
  int32_t height;
} OS_WindowResizeEvent;

typedef struct {
  OS_EventKind kind;
  union {
    OS_KeyEvent key;
    OS_MouseMoveEvent mouseMove;
    OS_WindowResizeEvent resize;
  };
} OS_Event;

OS_Window *OS_CreateWindow(uint32_t windowWidth, uint32_t windowHeight);

void OS_BlitWindow(OS_Window *w);

void OS_DrawPixel(OS_Window *w, int32_t pixel, Color c);
void OS_DrawRect(OS_Window *w, Rect r, Color c);
void OS_DrawBitmap(OS_Window *w, Bitmap src, V2 pos);
void OS_DrawBitmapRotated(OS_Window *w, Bitmap src, V2 pos, double angle);

void OS_PollEvents(OS_Window *w, OS_Event **_events, int32_t *count);

#endif // !CORE_OS_H

#ifdef CORE_OS_IMPLEMENTATION
#include "os/window.c"
#endif // CORE_OS_IMPLEMENTATION
