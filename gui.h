#ifndef CORE_GUI_H
#define CORE_GUI_H

#include "common.h"
#include "os.h"
#include "string.h"
#include <stdint.h>

#define DefineStack(type)                                                      \
  typedef struct type##_STACK_NODE {                                           \
    type data;                                                                 \
    struct type##_STACK_NODE *prev;                                            \
  } type##Stack;

typedef struct {
  uint64_t u64[1];
} UiKey;

typedef enum {
  UiWidgetFlag_None = (0 << 0),
  UiWidgetFlag_NoRender = (1 << 0),
  UiWidgetFlag_RenderText = (1 << 1),
  UiWidgetFlag_TextEdit = (1 << 2),
} UiWidgetFlags;

#define ShouldRenderWidget(w) ((Bool)((w)->flags & UiWidgetFlag_NoRender) == 0)
#define ShouldRenderText(w) ((Bool)((w)->flags & UiWidgetFlag_RenderText))
#define WidgetIsTextEdit(w) ((Bool)((w)->flags & UiWidgetFlag_TextEdit))

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

typedef struct {
  UiAxis layoutAxis;
  String text;
  Color bg;
  Color content;
  Bool isHot;
  Bool isActive;
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
  UiWidgetFlags flags;

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

DefineStack(Color);
DefineStack(UiSize);
DefineStack(UiAxis);

typedef struct {
  UiWidget *root;
  UiWidget *hot;
  UiWidget *active;

  float maxSize[UiAxis_COUNT];
  int32_t mouse[UiAxis_COUNT];
  Bool mouseClicked;
  String input;

  uint64_t widgetTableSize;
  UiWidgetHashSlot *widgetTable;

  ColorStack *bgColorStack;
  ColorStack *contentColorStack;
  UiSizeStack *widthStack;
  UiSizeStack *heightStack;
  UiAxisStack *layoutStack;
} UiContext;

typedef uint32_t UiSignalFlags;
enum {
  UiSignalFlags_Hovering = (1 << 0),
  UiSignalFlags_Clicked = (1 << 1),
};

typedef struct {
  UiWidget *w;
  UiSignalFlags f;
} UiSignal;

UiContext *GUICreateContext();
void GUI_HandleEvents(UiContext *ctx, OS_Event *events, int32_t count);
void GUI_SetMousePos(UiContext *ctx, int32_t mouseX, int32_t mouseY);

UiWidget *UiWidgetFromString(UiContext *ctx, String s, UiWidgetFlags flags);
UiSignal UiSignalFromWidget(UiContext *ctx, UiWidget *w);

void GUIBegin(UiContext *ctx);
void GUIEnd(UiContext *ctx, OS_Window *w);

void PushChildWidget(UiContext *ctx, UiWidget *w);
void PushParentWidget(UiContext *ctx, UiWidget *w);
UiWidget *PopParentWidget(UiContext *ctx);

/* TRANSIENT UI STATES */

void PushBgColor(UiContext *ctx, Color c);
void PopBgColor(UiContext *ctx);
void PushContentColor(UiContext *ctx, Color c);
void PopContentColor(UiContext *ctx);

void PushWidth(UiContext *ctx, UiSize s);
void PopWidth(UiContext *ctx);
void PushHeight(UiContext *ctx, UiSize s);
void PopHeight(UiContext *ctx);

void PushLayoutAxis(UiContext *ctx, UiAxis a);
void PopLayoutAxis(UiContext *ctx);

#define GUI_BgColor(ctx, c)                                                    \
  CORE_Defer(PushBgColor((ctx), (c)), PopBgColor((ctx)))
#define GUI_ContentColor(ctx, c)                                               \
  CORE_Defer(PushContentColor((ctx), (c)), PopContentColor((ctx)))
#define GUI_Width(ctx, c) CORE_Defer(PushWidth((ctx), (c)), PopWidth((ctx)))
#define GUI_Height(ctx, c) CORE_Defer(PushHeight((ctx), (c)), PopHeight((ctx)))
#define GUI_Layout(ctx, c) CORE_Defer(PushLayout((ctx), (c)), PopLayout((ctx)))

/* UI_WIDGETS */

UiWidget *GUI_RowBegin(UiContext *ctx, String name);
UiSignal GUI_RowEnd(UiContext *ctx);
UiWidget *GUI_ColumnBegin(UiContext *ctx, String name);
UiSignal GUI_ColumnEnd(UiContext *ctx);
UiSignal GUI_Box(UiContext *ctx, String name);
UiSignal GUI_Button(UiContext *ctx, String text);
UiSignal GUI_Text(UiContext *ctx, String text);
UiSignal GUI_TextEdit(UiContext *ctx, String *text);

#define GUI_Row(c, n) CORE_Defer(GUI_RowBegin((c), (n)), GUI_RowEnd((c)))
#define GUI_Column(c, n)                                                       \
  CORE_Defer(GUI_ColumnBegin((c), (n)), GUI_ColumnEnd((c)))

/* UI DATA MACROS */

#define GUI_Hovering(s) !!((s).f & UiSignalFlags_Hovering)
#define GUI_Clicked(s) !!((s).f & UiSignalFlags_Clicked)

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

#ifdef CORE_GUI_IMPLEMENTATION

#include "gui/gui_core.c"
#include "gui/gui_widget.c"

#endif // CORE_GUI_IMPLEMENTATION
#endif // !CORE_GUI_H
