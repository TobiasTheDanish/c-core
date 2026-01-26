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

typedef struct V3 {
  float x;
  float y;
  float z;
} V3;

typedef union M4x4f {
  struct {
    float _00; // col 0
    float _10;
    float _20;
    float _30;
    float _01; // col 1
    float _11;
    float _21;
    float _31;
    float _02; // col 2
    float _12;
    float _22;
    float _32;
    float _03; // col 3
    float _13;
    float _23;
    float _33;
  };
  float values[16];
} M4f;

typedef union {
  V2 pos;
  struct {
    float x;
    float y;
    float w;
    float h;
  };
} Rect;

typedef struct {
  float width;
  float height;
} CORE_Dimensions;

#define CORE_Defer(pre, post)                                                  \
  for (int __i__ = ((pre), 0); !__i__; __i__ += 1, (post))
// for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#endif
