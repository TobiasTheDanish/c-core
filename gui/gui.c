#include "../gui.h"
#include <assert.h>
#include <stdint.h>

void CalcWidgetSizes(UiWidget *w);
void CalcStandaloneSizes(UiWidget *w);
void CalcUpwardDependentSizes(UiWidget *w);
void CalcDownwardDependentSizes(UiWidget *w);

void SolveLayoutCollisions(UiWidget *w);

void CalcRelPositions(UiWidget *w);

void BeginDrawing(UiContext *ctx) {
  *ctx->root = (UiWidget){0};
  for (int axis = 0; axis < UiAxis_COUNT; axis++) {
    ctx->root->sizes[axis] = (UiSize){
        .kind = UiSizeKind_PIXELS,
        .value = ctx->maxSize[axis],
        .strictness = 1.0,
    };
    ctx->root->dimensions[axis] = ctx->maxSize[axis];
  }
}

void PushChildWidget(UiContext *ctx, UiWidget *w) {
  assert(ctx->root != 0);

  if (ctx->root->rightMost) {
    ctx->root->rightMost->left = w;
  }
  if (!ctx->root->right) {
    ctx->root->right = w;
  }
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

void EndDrawing(UiContext *ctx) { CalcWidgetSizes(ctx->root); }

void CalcWidgetSizes(UiWidget *w) {
  CalcStandaloneSizes(w);
  CalcUpwardDependentSizes(w);

  UiWidget *child = w->right;
  while (child) {
    CalcWidgetSizes(child);
    child = child->left;
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
  for (int axis = UiAxis_X; axis < UiAxis_COUNT; axis++) {
    UiSize axisSize = w->sizes[axis];
    switch (axisSize.kind) {
    case UiSizeKind_PERCENTOFPARENT: {
      UiWidget *parent = w->parent;
      while (parent) {
        if (parent->sizes[axis].kind == UiSizeKind_CHILDRENMAX ||
            parent->sizes[axis].kind == UiSizeKind_CHILDRENSUM) {
          parent = parent->parent;
          continue;
        }

        break;
      }
      assert(parent != 0 && "ALL PARENTS RELIED ON CHILDREN SIZES");
      w->dimensions[axis] = parent->dimensions[axis] * (axisSize.value / 100);
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
        child = child->left;
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
        child = child->left;
      }
      w->dimensions[axis] = sum;
    } break;

    default:
      break;
    }
  }
}
