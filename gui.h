#ifndef CORE_GUI_H
#define CORE_GUI_H

#include "os.h"
#include "string.h"
#include "types.h"
#include <stdint.h>

typedef struct {
  uint64_t u64[1];
} UiKey;

typedef enum {
  UiWidgetFlag_Container = (1 << 0),
  UiWidgetFlag_DisplayText = (1 << 1),
} UiWidgetFlags;

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

  struct UI_WIDGET *hash_next;
  struct UI_WIDGET *hash_prev;

  UiKey key;

  UiSize sizes[UiAxis_COUNT];

  float relativePosition[UiAxis_COUNT];
  float dimensions[UiAxis_COUNT];
  Rect screenRect;

  UiWidgetData data;
} UiWidget;

typedef struct {
  UiWidget *hash_first;
  UiWidget *hash_last;
} UiWidgetHashSlot;

typedef struct {
  UiWidget *root;
  float maxSize[UiAxis_COUNT];

  uint64_t widgetTableSize;
  UiWidgetHashSlot *widgetTable;
} UiContext;

UiContext *GUICreateContext();
UiWidget *UiWidgetFromString(UiContext *ctx, String s);

void GUIBegin(UiContext *ctx);
void GUIEnd(UiContext *ctx, OS_Window *w);

void PushChildWidget(UiContext *ctx, UiWidget *w);
void PushParentWidget(UiContext *ctx, UiWidget *w);
UiWidget *PopParentWidget(UiContext *ctx);

/* UI_WIDGETS */

UiWidget *GUI_RowBegin(UiContext *ctx, String name);
void GUI_RowEnd(UiContext *ctx);
UiWidget *GUI_ColumnBegin(UiContext *ctx, String name);
void GUI_ColumnEnd(UiContext *ctx);

#define GUI_UiSize(k, v, s)                                                    \
  (UiSize) { .kind = (k), .value = (v), .strictness = (s), }

#define GUI_WidgetSize(w, axis) (w).sizes[(axis)]
#define GUI_ContainerData(axis)                                                \
  (UiWidgetData) {                                                             \
    .kind = UiWidgetDataKind_Container, .container = {                         \
      .layoutAxis = (axis),                                                    \
    }                                                                          \
  }
#define GUI_ContainerBgColor(w) (w).data.container.bg
#define GUI_TextWidget()                                                       \
  (UiWidget) {                                                                 \
    .data = {.kind = UiWidgetDataKind_Text, .text = {0} }                      \
  }
#define GUI_TextBgColor(w) (w).data.text.bg

#ifdef GUI_IMPLEMENTATION

#include "gui/gui_core.c"
#include "gui/gui_widget.c"

#endif // GUI_IMPLEMENTATION
#endif // !CORE_GUI_H
