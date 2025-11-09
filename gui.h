#ifndef CORE_GUI_H
#define CORE_GUI_H

#include "string.h"
#include "types.h"
#include <stdint.h>

typedef enum {
  UiAxis_X = 0,
  UiAxis_Y,
  UiAxis_COUNT,
} UiAxis;

typedef enum {
  UiSizeKind_PIXELS = (1 << 0),
  UiSizeKind_TEXT = (1 << 1),
  UiSizeKind_PERCENTOFPARENT = (1 << 2),
  UiSizeKind_CHILDRENSUM = (1 << 3),
  UiSizeKind_CHILDRENMAX = (1 << 4),
} UiSizeKind;

#define DownwardDependentKind (UiSizeKind_CHILDRENMAX | UiSizeKind_CHILDRENSUM)

typedef struct {
  UiSizeKind kind;
  float value;
  float strictness;
} UiSize;

typedef enum {
  UiWidgetDataKind_None = 0,
  UiWidgetDataKind_Text,
  UiWidgetDataKind_Container,
} UiWidgetDataKind;

typedef struct {
  UiWidgetDataKind kind;
  union {
    struct {
    } none;
    struct {
      String value;
      Color bg;
      Color text;
    } text;
    struct {
      UiAxis layoutAxis;
      Color bg;
    } container;
  };
} UiWidgetData;

typedef struct UI_WIDGET {
  struct UI_WIDGET *parent;
  struct UI_WIDGET *nextSibling; // siblings
  struct UI_WIDGET *prevSibling; // siblings
  struct UI_WIDGET *right;       // children
  struct UI_WIDGET *rightMost;   // last child

  UiSize sizes[UiAxis_COUNT];

  float relativePosition[UiAxis_COUNT];
  float dimensions[UiAxis_COUNT];
  Rect screenRect;

  UiWidgetData data;
} UiWidget;

typedef struct {
  UiWidget *root;
  float maxSize[UiAxis_COUNT];
} UiContext;

void GUIBegin(UiContext *ctx, UiWidget *root);
void GUIEnd(UiContext *ctx);

void PushChildWidget(UiContext *ctx, UiWidget *w);
void PushParentWidget(UiContext *ctx, UiWidget *w);
void PopParentWidget(UiContext *ctx);

#define GUI_UiSize(k, v, s)                                                    \
  (UiSize) { .kind = (k), .value = (v), .strictness = (s), }

#define GUI_WidgetSize(w, axis) (w).sizes[(axis)]
#define GUI_ContainerWidget(axis)                                              \
  (UiWidget) {                                                                 \
    .data =                                                                    \
    {.kind = UiWidgetDataKind_Container,                                       \
     .container = {                                                            \
         .layoutAxis = (axis),                                                 \
     } }                                                                       \
  }
#define GUI_ContainerBgColor(w) (w).data.container.bg
#define GUI_TextWidget()                                                       \
  (UiWidget) {                                                                 \
    .data = {.kind = UiWidgetDataKind_Text, .text = {0} }                      \
  }
#define GUI_TextBgColor(w) (w).data.text.bg

#ifdef GUI_IMPLEMENTATION

#include "gui/gui.c"

#endif // GUI_IMPLEMENTATION
#endif // !CORE_GUI_H
