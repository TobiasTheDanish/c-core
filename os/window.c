#ifndef CORE_OS_WINDOW_C
#define CORE_OS_WINDOW_C
#include "../allocator.h"
#include "../os.h"
#include <math.h>

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

#include "opengl.c"

#define RGFW_IMPLEMENTATION
#define RGFW_OPENGL
#include "../thirdparty/RGFW.h"

Arena staticArena = {0};

void __AllocBitmap(Bitmap *b, int32_t width, int32_t height) {
  b->bpp = 4;
  int dataInBytes = width * height * b->bpp;

  b->width = width;
  b->height = height;
  b->stride = b->width;
  b->data = AllocMemory(dataInBytes);
}

OS_Window *OS_CreateWindow(uint32_t windowWidth, uint32_t windowHeight) {
  OS_Window *win = AllocMemory(sizeof(OS_Window));

  win->w =
      RGFW_createWindow("Window", RGFW_RECT(0, 0, windowWidth, windowHeight),
                        RGFW_windowCenter | RGFW_windowOpenglSoftware);

  __AllocBitmap(&win->frameBitmap, windowWidth, windowHeight);

  // OPEN GL INITIALIZATION
  OPENGL_init();
  glViewport(0, 0, windowWidth, windowHeight);

  return win;
}

void __AppendEvent(OS_Event **events, int *count, OS_Event e) {
  if (events == 0) {
    return;
  }

  if (*events == 0) {
    *events = AllocMemory(sizeof(OS_Event));
    (*events)[0] = e;
    *count = 1;
  } else {
    *events = ReallocMemory(*events, ((*count) + 1) * sizeof(OS_Event));
    (*events)[*count] = e;
    *count += 1;
  }
}

void OS_PollEvents(OS_Window *w, OS_Event **_events, int32_t *count) {
  for (RGFW_event *event = RGFW_window_checkEvent(w->w); event != 0;
       event = RGFW_window_checkEvent(w->w)) {
    OS_Event e = {0};
    switch (event->type) {
    case RGFW_windowResized: {
      int32_t width = w->w->r.w;
      int32_t height = w->w->r.h;
      glViewport(0, 0, width, height);

      FreeMemory(w->frameBitmap.data);
      __AllocBitmap(&w->frameBitmap, width, height);

      e = (OS_Event){
          .kind = OS_EventKind_WindowResize,
          .resize =
              {
                  .width = width,
                  .height = height,
              },
      };
    } break;

    case RGFW_mouseButtonPressed: {
      e = (OS_Event){
          .kind = OS_EventKind_KeyDown,
          .key =
              {
                  .keyCode = OS_KeyCode_mouseLeft + event->button,
              },
      };
    } break;
    case RGFW_mouseButtonReleased: {
      e = (OS_Event){
          .kind = OS_EventKind_KeyUp,
          .key =
              {
                  .keyCode = OS_KeyCode_mouseLeft + event->button,
              },
      };
    } break;

    case RGFW_keyPressed:
      e = (OS_Event){
          .kind = OS_EventKind_KeyDown,
          .key =
              {
                  .keyCode = event->key,
              },
      };
      break;
    case RGFW_keyReleased:
      e = (OS_Event){
          .kind = OS_EventKind_KeyUp,
          .key =
              {
                  .keyCode = event->key,
              },
      };
      break;
    case RGFW_mousePosChanged:
      e = (OS_Event){
          .kind = OS_EventKind_MouseMove,
          .mouseMove =
              {
                  .x = event->point.x,
                  .y = event->point.y,
              },
      };
      break;
    case RGFW_quit:
      e = (OS_Event){
          .kind = OS_EventKind_Quit,
      };
      break;
    }

    __AppendEvent(_events, count, e);
  }
}

void OS_BlitWindow(OS_Window *w) {
  // Use w->frameBitmap to render to OPENGL texture
  OPENGL_RenderBitmap(w->frameBitmap);

  RGFW_window_swapBuffers(w->w);
  ZeroMemory(w->frameBitmap.data);
}

void OS_DrawPixel(OS_Window *w, int32_t pixel, Color c) {
  if (c.a == 0) {
    return;
  }

  w->frameBitmap.data[pixel] = c.b;
  w->frameBitmap.data[pixel + 1] = c.g;
  w->frameBitmap.data[pixel + 2] = c.r;
  w->frameBitmap.data[pixel + 3] = c.a;
}

void OS_DrawRect(OS_Window *w, Rect r, Color c) {
  int32_t startX = fmax(r.x, 0);
  int32_t startY = fmax(r.y, 0);

  int32_t targetX = fmin(r.x + r.w, w->frameBitmap.width);
  int32_t targetY = fmin(r.y + r.h, w->frameBitmap.height);

  int32_t maxPixel =
      w->frameBitmap.width * w->frameBitmap.height * w->frameBitmap.bpp;
  for (int y = startY; y < targetY; y++) {
    for (int x = startX; x < targetX; x++) {
      int32_t pixel = (y * w->frameBitmap.width + x) * w->frameBitmap.bpp;
      if (maxPixel <= pixel || pixel <= 0) {
        continue;
      }

      OS_DrawPixel(w, pixel, c);
    }
  }
}

void OS_DrawBitmapRotated(OS_Window *w, Bitmap src, V2 pos, double angle) {
  float cosAng = cos(angle);
  float sinAng = sin(angle);

  int32_t startX = fmax(pos.x, 0);
  int32_t startY = fmax(pos.y, 0);

  int32_t cx = pos.x;
  int32_t cy = pos.y;

  int32_t targetX = fmin(pos.x + src.width, w->frameBitmap.width);
  int32_t targetY = fmin(pos.y + src.height, w->frameBitmap.height);

  int32_t maxPixel =
      w->frameBitmap.width * w->frameBitmap.height * w->frameBitmap.bpp;
  for (int y = startY; y < targetY; y++) {
    for (int x = startX; x < targetX; x++) {
      int32_t newX =
          (int32_t)(cosAng * (float)(x - cx) - sinAng * (float)(y - cy)) +
          pos.x;
      int32_t newY =
          (int32_t)(sinAng * (float)(x - cx) + cosAng * (float)(y - cy)) +
          pos.y;

      int32_t pixel = (newY * w->frameBitmap.width + newX) * w->frameBitmap.bpp;
      if (maxPixel <= pixel || pixel <= 0) {
        continue;
      }

      int32_t srcPixel = ((y - startY) * src.width + (x - startX)) * src.bpp;
      Color c = {0};
      for (int i = 0; i < src.bpp; i++) {
        c.bgra |= (src.data[srcPixel + i] << (i * 8));
      }

      OS_DrawPixel(w, pixel, c);
    }
  }
}

void OS_DrawBitmap(OS_Window *w, Bitmap src, V2 pos) {
  OS_DrawBitmapRotated(w, src, pos, 0.0);
}
#endif /* ifndef CORE_OS_WINDOW_C */
