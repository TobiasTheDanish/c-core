#ifndef CORE_RENDER_H
#define CORE_RENDER_H
#include "types.h"

void CreateWindow(uint32_t windowWidth, uint32_t windowHeight);
void RendererWindowResize(uint32_t width, uint32_t height);
void RendererBlit();

void DrawPixel(int32_t pixel, Color c);
void DrawRect(Rect r, Color c);
void DrawBitmap(Bitmap src, V2 pos);
void DrawBitmapRotated(Bitmap src, V2 pos, double angle);

#ifdef RENDER_IMPLEMENTATION
#include "render/render.c"
#endif // RENDER_IMPLEMENTATION

#endif // !CORE_RENDER_H
