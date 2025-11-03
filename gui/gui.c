#include "../gui.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void CalcWidgetSizes(UiWidget *w);
void CalcStandaloneSizes(UiWidget *w);
void CalcUpwardDependentSizes(UiWidget *w);
void CalcDownwardDependentSizes(UiWidget *w);

void SolveLayoutCollisions(UiWidget *w);

void CalcRelPositions(UiWidget *w);

void BeginDrawing(UiContext *ctx, UiWidget *root) {
  ctx->root = root;
  for (int axis = 0; axis < UiAxis_COUNT; axis++) {
    ctx->root->sizes[axis] = (UiSize){
        .kind = UiSizeKind_PIXELS,
        .value = ctx->maxSize[axis],
        .strictness = 1.0,
    };
    ctx->root->dimensions[axis] = ctx->maxSize[axis];
  }
}

void EndDrawing(UiContext *ctx) {
  CalcWidgetSizes(ctx->root);
  SolveLayoutCollisions(ctx->root);
  CalcRelPositions(ctx->root);
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

void PushParentWidget(UiContext *ctx, UiWidget *w) {
  assert(ctx->root != 0);

  PushChildWidget(ctx, w);
  ctx->root = w;
}

void PopParentWidget(UiContext *ctx) {
  assert(ctx->root->parent != 0);
  ctx->root = ctx->root->parent;
}

void CalcWidgetSizes(UiWidget *w) {
  CalcStandaloneSizes(w);
  CalcUpwardDependentSizes(w);

  UiWidget *child = w->right;
  while (child) {
    CalcWidgetSizes(child);
    child = child->nextSibling;
  }

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
      while (child) {
        if (child->dimensions[axis] > max) {
          max = child->dimensions[axis];
        }
        child = child->nextSibling;
      }
      w->dimensions[axis] = max;
    } break;
    case UiSizeKind_CHILDRENSUM: {
      float sum = -1;
      UiWidget *child = w->right;
      while (child) {
        if (child->dimensions[axis] > sum) {
          sum += child->dimensions[axis];
        }
        child = child->nextSibling;
      }
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
  UiWidget *child;

  if (w->data.kind == UiWidgetDataKind_Container) {
    UiAxis axis = w->data.container.layoutAxis;
    float TotalChildDim = 0;
    child = w->right;
    while (child) {
      TotalChildDim += child->dimensions[axis];
      child = child->nextSibling;
    }

    if (TotalChildDim > w->dimensions[axis]) {
      float violation = TotalChildDim - w->dimensions[axis];
      child = w->right;
      while (child) {
        float maxSizeDecrease =
            (child->dimensions[axis] * (1.0 - child->sizes[axis].strictness));
        float decrease =
            maxSizeDecrease >= violation ? violation : maxSizeDecrease;

        child->dimensions[axis] -= decrease;
        violation -= decrease;

        child = child->nextSibling;
      }
    }

    child = w->right;
    while (child) {
      SolveLayoutCollisions(child);

      child = child->nextSibling;
    }

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

  UiWidget *child = w->right;
  while (child) {
    CalcRelPositions(child);

    child = child->nextSibling;
  }
}
