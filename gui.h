#ifndef CORE_GUI_H
#define CORE_GUI_H

typedef struct {
  float x;
  float y;
  float w;
  float h;
} Rect;

typedef enum {
  UiAxis_X = 0,
  UiAxis_Y,
  UiAxis_COUNT,
} UiAxis;

typedef enum {
  UiSizeKind_PIXELS,
  UiSizeKind_TEXT,
  UiSizeKind_PERCENTOFPARENT,
  UiSizeKind_CHILDRENSUM,
  UiSizeKind_CHILDRENMAX,
} UiSizeKind;

typedef struct {
  UiSizeKind kind;
  float value;
  float strictness;
} UiSize;

typedef struct UI_WIDGET {
  struct UI_WIDGET *parent;
  struct UI_WIDGET *left;      // siblings
  struct UI_WIDGET *right;     // children
  struct UI_WIDGET *rightMost; // last child

  UiSize sizes[UiAxis_COUNT];

  float relativePosition[UiAxis_COUNT];
  float dimensions[UiAxis_COUNT];
  Rect screenRect;
} UiWidget;

typedef struct {
  UiWidget *root;
  float maxSize[UiAxis_COUNT];
} UiContext;

void BeginDrawing(UiContext *ctx);
void EndDrawing(UiContext *ctx);

void PushChildWidget(UiContext *ctx, UiWidget *w);
void PushParentWidget(UiContext *ctx, UiWidget *w);
void PopParentWidget(UiContext *ctx);

#endif // !CORE_GUI_H
#ifdef GUI_IMPLEMENTATION
#ifndef IMPLEMENTATION_GUARD
#define IMPLEMENTATION_GUARD

#include "gui/gui.c"

#endif // !IMPLEMENTATION_GUARD
#endif // GUI_IMPLEMENTATION
