#include <stdio.h>
#define RENDER_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#include "../gui.h"
#define TEST_IMPLEMENTATION
#include "../testing.h"

int main() {
  UiContext *ctx = GUICreateContext();
  ctx->maxSize[UiAxis_X] = 100;
  ctx->maxSize[UiAxis_Y] = 100;

  GUIBegin(ctx);

  UiWidget *row1 = GUI_RowBegin(ctx, StringFromCString("row1"));
  assertEqualsPtr(ctx->root, row1);
  GUI_WidgetSize(*row1, UiAxis_X) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

  GUI_WidgetSize(*row1, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 50, 1);

  // insert children
  UiWidget *box1 = GUI_RowBegin(ctx, StringFromCString("box1"));
  GUI_WidgetSize(*box1, UiAxis_X) = GUI_UiSize(UiSizeKind_PIXELS, 40, 1);

  GUI_WidgetSize(*box1, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);
  GUI_RowEnd(ctx);
  assertEqualsPtr(ctx->root->right, box1);
  assertEqualsPtr(ctx->root->rightMost, box1);
  UiWidget *box2 = GUI_RowBegin(ctx, StringFromCString("box2"));
  GUI_WidgetSize(*box2, UiAxis_X) = GUI_UiSize(UiSizeKind_PIXELS, 20, 1);

  GUI_WidgetSize(*box2, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);
  GUI_RowEnd(ctx);
  assertEqualsPtr(ctx->root->right, box1);
  assertEqualsPtr(ctx->root->rightMost, box2);

  UiWidget *box3 = GUI_RowBegin(ctx, StringFromCString("box3"));
  GUI_WidgetSize(*box3, UiAxis_X) = GUI_UiSize(UiSizeKind_PIXELS, 40, 1);

  GUI_WidgetSize(*box3, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);
  GUI_RowEnd(ctx);
  assertEqualsPtr(ctx->root->right, box1);
  assertEqualsPtr(ctx->root->rightMost, box3);

  GUI_RowEnd(ctx);
  assertNotEqualsPtr(ctx->root, row1);

  UiWidget *row2 = GUI_RowBegin(ctx, StringFromCString("row2"));
  assertEqualsPtr(ctx->root, row2);
  GUI_WidgetSize(*row2, UiAxis_X) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);

  GUI_WidgetSize(*row2, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 50, 1);

  UiWidget *box4 = GUI_RowBegin(ctx, StringFromCString("box4"));
  // insert children
  GUI_WidgetSize(*box4, UiAxis_X) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 75, 0.666666666666);

  GUI_WidgetSize(*box4, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);
  GUI_RowEnd(ctx);
  assertEqualsPtr(box4->parent, row2);
  assertEqualsPtr(ctx->root->right, box4);
  assertEqualsPtr(ctx->root->rightMost, box4);

  UiWidget *box5 = GUI_RowBegin(ctx, StringFromCString("box5"));
  GUI_WidgetSize(*box5, UiAxis_X) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 75, 0.666666666666);

  GUI_WidgetSize(*box5, UiAxis_Y) =
      GUI_UiSize(UiSizeKind_PERCENTOFPARENT, 100, 1);
  GUI_RowEnd(ctx);
  assertEqualsPtr(box5->parent, row2);
  assertEqualsPtr(ctx->root->right, box4);
  assertEqualsPtr(ctx->root->rightMost, box5);
  assertEqualsPtr(box4, box5->prevSibling);
  assertEqualsPtr(box5, box4->nextSibling);

  GUI_RowEnd(ctx);
  assertNotEqualsPtr(ctx->root, &row2);

  printf("WIDGET SUCCESS\n");

  GUIEnd(ctx);

  UiWidget *cur = ctx->root->right;

  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("row1"))));

  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 100);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->right;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("box1"))));

  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 40);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("box2"))));
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 40);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 40);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 20);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("box3"))));
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 60);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 60);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 40);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->parent->nextSibling;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("row2"))));
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 50);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 100);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->right;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("box4"))));
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 50);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertTrue(UiKeyMatch(cur->key, UiKeyFromString(UiWidgetSeedKey(cur->parent),
                                                  StringFromCString("box5"))));
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 50);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 50);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 50);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  printf("GUIEnd SUCCESS\n");
}
