#include "../gui.h"
#include "../os.h"
#include <assert.h>
#include <stdint.h>

#define ForEachChild(name, w)                                                  \
  for (UiWidget *name = (w)->right; name != 0; name = name->nextSibling)

uint64_t U64HashFromString(uint64_t hash, String s) {
  int c;

  while ((c = *s++) != 0) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

Bool UiKeyMatch(UiKey a, UiKey b) { return a.u64[0] == b.u64[0]; }

UiKey UiZeroKey() {
  UiKey res = {0};
  return res;
}

UiKey UiKeyFromString(UiKey seed, String s) {
  UiKey result = {0};
  if (StringLength(s) != 0) {
    result.u64[0] = U64HashFromString(seed.u64[0], s);
  }
  return result;
}

UiWidget *UiWidgetFromKey(UiContext *ctx, UiKey key) {
  UiWidget *res = 0;

  uint64_t slot = key.u64[0] % ctx->widgetTableSize;
  for (UiWidget *w = ctx->widgetTable[slot].hash_first; w != 0;
       w = w->hash_next) {
    if (UiKeyMatch(key, w->key)) {
      res = w;
      break;
    }
  }

  return res;
}

UiKey UiWidgetSeedKey(UiWidget *p) {
  UiWidget *keyedParent = 0;
  for (; p != 0; p = p->parent) {
    if (!UiKeyMatch(p->key, UiZeroKey())) {
      keyedParent = p;
      break;
    }
  }

  return keyedParent != 0 ? keyedParent->key : UiZeroKey();
}

UiKey UiActiveSeedKey(UiContext *ctx) { return UiWidgetSeedKey(ctx->root); }

/** The String passed through 's' param is freed in this function */
UiWidget *UiWidgetFromString(UiContext *ctx, String s) {
  UiKey key = UiKeyFromString(UiActiveSeedKey(ctx), s);
  StringFree(s);
  UiWidget *w = UiWidgetFromKey(ctx, key);
  Bool firstFrame = w == 0;

  if (firstFrame) {
    w = AllocMemory(sizeof(UiWidget));
  }

  w->key = key;
  w->parent = w->prevSibling = w->nextSibling = w->right = w->rightMost = 0;

  if (ctx->root) {
    PushChildWidget(ctx, w);
  }

  return w;
}

Bool Ui_PointInRect(int32_t px, int32_t py, Rect r) {
  return ((r.x <= px && px <= r.x + r.w) && (r.y <= py && py <= r.y + r.h));
}

UiSignal UiSignalFromWidget(UiContext *ctx, UiWidget *w) {
  UiSignal s = {0};
  if (Ui_PointInRect(ctx->mouse[UiAxis_X], ctx->mouse[UiAxis_Y],
                     w->screenRect)) {
    s.f |= UiSignalFlags_Hovering;
  }

  return s;
}

void CalcWidgetSizes(UiWidget *w);
void CalcStandaloneSizes(UiWidget *w);
void CalcUpwardDependentSizes(UiWidget *w);
void CalcDownwardDependentSizes(UiWidget *w);

void SolveLayoutCollisions(UiWidget *w);

void CalcRelPositions(UiWidget *w);

void RenderWidgetTree(UiWidget *w, OS_Window *win);

void ResetWidgetCache(UiContext *ctx);
void UpdateWidgetCache(UiContext *ctx, UiWidget *w);

UiContext *GUICreateContext() {
  UiContext *ctx = AllocMemory(sizeof(UiContext));

  ctx->widgetTableSize = 4096;
  ctx->widgetTable =
      AllocMemory(ctx->widgetTableSize * sizeof(UiWidgetHashSlot));

  return ctx;
}

void GUI_SetMousePos(UiContext *ctx, int32_t mouseX, int32_t mouseY) {
  ctx->mouse[UiAxis_X] = mouseX;
  ctx->mouse[UiAxis_Y] = mouseY;
}

void GUIBegin(UiContext *ctx) {
  String s = StringFromCString("root");
  UiKey key = UiKeyFromString(UiZeroKey(), s);
  StringFree(s);
  UiWidget *root = UiWidgetFromKey(ctx, key);
  Bool firstFrame = root == 0;

  if (firstFrame) {
    root = AllocMemory(sizeof(UiWidget));
  }

  root->key = key;
  root->parent = root->prevSibling = root->nextSibling = root->right =
      root->rightMost = 0;

  root->data = GUI_ContainerData(UiAxis_Y);

  PushParentWidget(ctx, root);
  GUI_ContainerBgColor(*root) = NewColor(.bgra = 0xFFFFFFFF);

  for (int axis = 0; axis < UiAxis_COUNT; axis++) {
    ctx->root->sizes[axis] = (UiSize){
        .kind = UiSizeKind_PIXELS,
        .value = ctx->maxSize[axis],
        .strictness = 1.0,
    };
    ctx->root->dimensions[axis] = ctx->maxSize[axis];
  }
}

void GUIEnd(UiContext *ctx, OS_Window *w) {
  CalcWidgetSizes(ctx->root);
  SolveLayoutCollisions(ctx->root);
  CalcRelPositions(ctx->root);
  RenderWidgetTree(ctx->root, w);
  ResetWidgetCache(ctx);
  UpdateWidgetCache(ctx, ctx->root);
}

void PushChildWidget(UiContext *ctx, UiWidget *w) {
  assert(ctx->root != 0);

  w->parent = ctx->root;
  if (ctx->root->rightMost) {
    ctx->root->rightMost->nextSibling = w;
  }
  if (!ctx->root->right) {
    ctx->root->right = w;
  }
  w->prevSibling = ctx->root->rightMost;
  ctx->root->rightMost = w;
}

void PushParentWidget(UiContext *ctx, UiWidget *w) { ctx->root = w; }

UiWidget *PopParentWidget(UiContext *ctx) {
  assert(ctx->root->parent != 0);
  UiWidget *w = ctx->root;
  ctx->root = ctx->root->parent;

  return w;
}

void CalcWidgetSizes(UiWidget *w) {
  CalcStandaloneSizes(w);
  CalcUpwardDependentSizes(w);

  ForEachChild(child, w) { CalcWidgetSizes(child); }

  CalcDownwardDependentSizes(w);
}
void CalcStandaloneSizes(UiWidget *w) {
  for (int axis = UiAxis_X; axis < UiAxis_COUNT; axis++) {
    UiSize axisSize = w->sizes[axis];
    switch (axisSize.kind) {
    case UiSizeKind_PIXELS:
    case UiSizeKind_TEXT:
      w->dimensions[axis] = axisSize.value;
      break;

    default:
      break;
    }
  }
}
void CalcUpwardDependentSizes(UiWidget *w) {
  for (UiAxis axis = UiAxis_X; axis < UiAxis_COUNT; axis++) {
    switch (w->sizes[axis].kind) {
    case UiSizeKind_PERCENTOFPARENT: {
      UiWidget *parent = w->parent;
      while (parent) {
        if (parent->sizes[axis].kind & DownwardDependentKind) {
          parent = parent->parent;
          continue;
        }

        break;
      }
      assert(parent != 0 && "ALL PARENTS RELIED ON CHILDREN SIZES");
      w->dimensions[axis] =
          parent->dimensions[axis] * (w->sizes[axis].value / 100);
    } break;

    default:
      break;
    }
  }
}
void CalcDownwardDependentSizes(UiWidget *w) {
  if (w->right == 0) {
    return;
  }

  for (int axis = UiAxis_X; axis < UiAxis_COUNT; axis++) {
    switch (w->sizes[axis].kind) {
    case UiSizeKind_CHILDRENMAX: {
      float max = -1;
      UiWidget *child = w->right;
      ForEachChild(child, w) {
        if (child->dimensions[axis] > max) {
          max = child->dimensions[axis];
        }
      }
      w->dimensions[axis] = max;
    } break;
    case UiSizeKind_CHILDRENSUM: {
      float sum = 0;
      ForEachChild(child, w) { sum += child->dimensions[axis]; }
      w->dimensions[axis] = sum;
    } break;

    default:
      break;
    }
  }
}

Bool WidgetIsDownwardDependent(UiWidget *w) {
  UiSizeKind xKind = w->sizes[UiAxis_X].kind;
  UiSizeKind yKind = w->sizes[UiAxis_Y].kind;

  return xKind & DownwardDependentKind || yKind & DownwardDependentKind;
}

void SolveLayoutCollisions(UiWidget *w) {
  if (w->right == 0) {
    return;
  }

  if (w->data.kind == UiWidgetDataKind_Container) {
    UiAxis axis = w->data.container.layoutAxis;
    float TotalChildDim = 0;
    ForEachChild(child, w) { TotalChildDim += child->dimensions[axis]; }

    if (TotalChildDim > w->dimensions[axis]) {
      float violation = TotalChildDim - w->dimensions[axis];
      ForEachChild(child, w) {
        float maxSizeDecrease =
            (child->dimensions[axis] * (1.0 - child->sizes[axis].strictness));
        float decrease =
            maxSizeDecrease >= violation ? violation : maxSizeDecrease;

        child->dimensions[axis] -= decrease;
        violation -= decrease;
      }
    }

    ForEachChild(child, w) { SolveLayoutCollisions(child); }

    if (WidgetIsDownwardDependent(w)) {
      CalcDownwardDependentSizes(w);
    }
  }
}

void CalcRelPositions(UiWidget *w) {
  if (w->prevSibling != 0 &&
      w->parent->data.kind == UiWidgetDataKind_Container) {
    UiAxis axis = w->parent->data.container.layoutAxis;
    w->relativePosition[axis] = w->prevSibling->relativePosition[axis] +
                                w->prevSibling->dimensions[axis];
  }

  w->screenRect = (Rect){
      .x = (w->parent ? w->parent->screenRect.x : 0) +
           w->relativePosition[UiAxis_X],
      .y = (w->parent ? w->parent->screenRect.y : 0) +
           w->relativePosition[UiAxis_Y],
      .w = w->dimensions[UiAxis_X],
      .h = w->dimensions[UiAxis_Y],
  };

  ForEachChild(child, w) { CalcRelPositions(child); }
}

void RenderWidget(UiWidget *w, OS_Window *win) {
  Color c;
  switch (w->data.kind) {
  case UiWidgetDataKind_Container:
    c = w->data.container.bg;
    break;
  case UiWidgetDataKind_Text:
    c = w->data.text.bg;
    break;
  case UiWidgetDataKind_None:
    return;
  }

  OS_DrawRect(win, w->screenRect, c);
}

void RenderWidgetTree(UiWidget *w, OS_Window *win) {
  RenderWidget(w, win);

  ForEachChild(child, w) { RenderWidgetTree(child, win); }
}

void ResetWidgetCache(UiContext *ctx) {
  for (int i = 0; i < ctx->widgetTableSize; i++) {
    UiWidgetHashSlot *slot = &(ctx->widgetTable[i]);
    slot->hash_first = 0;
    slot->hash_last = 0;
  }
}

void UpdateWidgetCache(UiContext *ctx, UiWidget *w) {
  uint64_t slotIdx = w->key.u64[0] % ctx->widgetTableSize;
  UiWidgetHashSlot *slot = &ctx->widgetTable[slotIdx];

  if (slot->hash_first == 0) {
    slot->hash_first = w;
  }

  w->hash_prev = slot->hash_last;
  if (slot->hash_last != 0) {
    slot->hash_last->hash_next = w;
  }

  slot->hash_last = w;

  ForEachChild(child, w) { UpdateWidgetCache(ctx, child); }
}
