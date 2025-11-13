#include <math.h>

#ifdef CUSTOM_ALLOCATOR
#define ALLOCATOR_IMPLEMENTATION
#include "../allocator.h"
#define ___AllocMemory(b) AllocMemory((b))
#else
#include <stdlib.h>
#define ___AllocMemory(b) malloc((b))
#endif
#include "../render.h"

void __AllocBitmap(Bitmap *b, int32_t width, int32_t height) {
  b->bpp = 4;
  int dataInBytes = width * height * b->bpp;

  b->width = width;
  b->height = height;
  b->stride = b->width;
  b->data = ___AllocMemory(dataInBytes);
}

Bitmap renderBitmap = {0};

#ifdef __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

Display *__display;
Visual *__visual;
int __depth;
Window __window;
Atom __wm_delete_window;
Pixmap __backbuffer;
GC __gc;
XImage *__windowImage;

// int __DestroyImage(XImage *i) {
//   FreeMemory(i->data);
//   FreeMemory(i);
//   return 0;
// }

XImage *__CreateImage(Display *d, Visual *visual, int depth, int width,
                      int height, char **buffer) {
  XImage *i =
      XCreateImage(d, visual, depth, ZPixmap, 0, *buffer, width, height, 32, 0);

  // i->f.destroy_image = __DestroyImage;

  return i;
}

void CreateWindow(uint32_t windowWidth, uint32_t windowHeight) {
  __display = XOpenDisplay(NULL);
  if (!__display) {
    fprintf(stderr, "Cannot open display\n");
    return;
  }

  int screen = DefaultScreen(__display);
  __visual = DefaultVisual(__display, screen);
  __depth = DefaultDepth(__display, screen);

  __window = XCreateSimpleWindow(__display, RootWindow(__display, screen), 50,
                                 50, windowWidth, windowHeight, 1,
                                 BlackPixel(__display, screen),
                                 WhitePixel(__display, screen));

  __backbuffer =
      XCreatePixmap(__display, __window, windowWidth, windowHeight, __depth);
  XSetWindowBackgroundPixmap(__display, __window, None); // Removes flickering

  __wm_delete_window = XInternAtom(__display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(__display, __window, &__wm_delete_window, 1);

  long eventMask =
      KeyPressMask | KeyReleaseMask | PointerMotionMask | StructureNotifyMask;

  XSelectInput(__display, __window, eventMask);
  XMapWindow(__display, __window);

  __gc = XCreateGC(__display, __window, 0, NULL);
  __AllocBitmap(&renderBitmap, windowWidth, windowHeight);

  __windowImage =
      __CreateImage(__display, __visual, __depth, renderBitmap.width,
                    renderBitmap.height, (char **)&renderBitmap.data);
}

void RendererWindowResize(uint32_t width, uint32_t height) {
  if (__windowImage && renderBitmap.data) {
    XDestroyImage(__windowImage);
  }
  __AllocBitmap(&renderBitmap, width, height);
  __windowImage =
      __CreateImage(__display, __visual, __depth, renderBitmap.width,
                    renderBitmap.height, (char **)&renderBitmap.data);

  XFreePixmap(__display, __backbuffer);
  __backbuffer = XCreatePixmap(__display, __window, width, height, __depth);
}

void RendererBlit() {
  XPutImage(__display, __backbuffer, __gc, __windowImage, 0, 0, 0, 0,
            renderBitmap.width, renderBitmap.height);
  XCopyArea(__display, __backbuffer, __window, __gc, 0, 0, renderBitmap.width,
            renderBitmap.height, 0, 0);
}

#endif /* ifdef __linux__ */

void DrawPixel(int32_t pixel, Color c) {
  if (c.a == 0) {
    return;
  }

  renderBitmap.data[pixel] = c.b;
  renderBitmap.data[pixel + 1] = c.g;
  renderBitmap.data[pixel + 2] = c.r;
  renderBitmap.data[pixel + 3] = c.a;
}

void DrawRect(Rect r, Color c) {
  int32_t startX = fmax(r.x, 0);
  int32_t startY = fmax(r.y, 0);

  int32_t targetX = fmin(r.x + r.w, renderBitmap.width);
  int32_t targetY = fmin(r.y + r.h, renderBitmap.height);

  int32_t maxPixel =
      renderBitmap.width * renderBitmap.height * renderBitmap.bpp;
  for (int y = startY; y < targetY; y++) {
    for (int x = startX; x < targetX; x++) {
      int32_t pixel = (y * renderBitmap.width + x) * renderBitmap.bpp;
      if (maxPixel <= pixel || pixel <= 0) {
        continue;
      }

      DrawPixel(pixel, c);
    }
  }
}

void DrawBitmapRotated(Bitmap src, V2 pos, double angle) {
  float cosAng = cos(angle);
  float sinAng = sin(angle);

  int32_t startX = fmax(pos.x - src.width * 0.5, 0);
  int32_t startY = fmax(pos.y - src.height * 0.5, 0);

  int32_t cx = pos.x;
  int32_t cy = pos.y;

  int32_t targetX = fmin(pos.x + src.width * 0.5, renderBitmap.width);
  int32_t targetY = fmin(pos.y + src.height * 0.5, renderBitmap.height);

  int32_t maxPixel =
      renderBitmap.width * renderBitmap.height * renderBitmap.bpp;
  for (int y = startY; y < targetY; y++) {
    for (int x = startX; x < targetX; x++) {
      int32_t newX =
          (int32_t)(cosAng * (float)(x - cx) - sinAng * (float)(y - cy)) +
          pos.x;
      int32_t newY =
          (int32_t)(sinAng * (float)(x - cx) + cosAng * (float)(y - cy)) +
          pos.y;

      int32_t pixel = (newY * renderBitmap.width + newX) * renderBitmap.bpp;
      if (maxPixel <= pixel || pixel <= 0) {
        continue;
      }

      int32_t srcPixel = ((y - startY) * src.width + (x - startX)) * src.bpp;
      Color c = {0};
      for (int i = 0; i < src.bpp; i++) {
        c.bgra |= (src.data[srcPixel + i] << (i * 8));
      }

      DrawPixel(pixel, c);
    }
  }
}

void DrawBitmap(Bitmap src, V2 pos) { DrawBitmapRotated(src, pos, 0.0); }
