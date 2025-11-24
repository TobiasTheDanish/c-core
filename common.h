#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdint.h>

typedef uint8_t Bool;
#define true ((Bool)1)
#define false ((Bool)0)

typedef union {
  struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };
  uint32_t bgra;
} Color;

#define NewColor(args...)                                                      \
  (Color) { args }

typedef struct BITMAP_STRUCT {
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint8_t bpp; // Bytes Per Pixel
  uint8_t *data;
} Bitmap;

typedef struct V2 {
  float x;
  float y;
} V2;

typedef union {
  V2 pos;
  struct {
    float x;
    float y;
    float w;
    float h;
  };
} Rect;

#define CORE_Defer(pre, post)                                                  \
  for (int __i__ = ((pre), 0); !__i__; __i__ += 1, (post))
// for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#endif
