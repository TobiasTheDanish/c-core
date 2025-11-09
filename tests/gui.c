#include <stdio.h>
#define GUI_IMPLEMENTATION
#include "../gui.h"
#define TEST_IMPLEMENTATION
#include "../testing.h"

int main() {
  UiContext ctx = {
      .maxSize =
          {
              [UiAxis_X] = 100,
              [UiAxis_Y] = 100,
          },
  };
  UiWidget column = {
      .data =
          {
              .kind = UiWidgetDataKind_Container,
              .container =
                  {
                      .layoutAxis = UiAxis_Y,
                  },
          },
  };
  GUIBegin(&ctx, &column);

  UiWidget row1 = {
      .sizes =
          {
              [UiAxis_X] =
                  {
                      .kind = UiSizeKind_PERCENTOFPARENT,
                      .value = 100,
                      .strictness = 1,
                  },
              [UiAxis_Y] =
                  {
                      .kind = UiSizeKind_PERCENTOFPARENT,
                      .value = 50,
                      .strictness = 1,
                  },
          },
      .data =
          {
              .kind = UiWidgetDataKind_Container,
              .container =
                  {
                      .layoutAxis = UiAxis_X,
                  },
          },
  };
  UiWidget row2 = row1;
  PushParentWidget(&ctx, &row1);
  assertEqualsPtr(ctx.root, &row1);

  // insert children
  UiWidget box1 = {
      .sizes =
          {
              [UiAxis_X] =
                  {
                      .kind = UiSizeKind_PIXELS,
                      .value = 40,
                      .strictness = 1,
                  },
              [UiAxis_Y] =
                  {
                      .kind = UiSizeKind_PERCENTOFPARENT,
                      .value = 100,
                      .strictness = 1,
                  },
          },
  };
  UiWidget box2 = box1;
  UiWidget box3 = box1;
  UiWidget box4 = box1;
  UiWidget box5 = box4;
  PushChildWidget(&ctx, &box1);
  assertEqualsPtr(ctx.root->right, &box1);
  assertEqualsPtr(ctx.root->rightMost, &box1);

  box2.sizes[UiAxis_X].value = 20;
  PushChildWidget(&ctx, &box2);
  assertEqualsPtr(ctx.root->rightMost, &box2);

  PushChildWidget(&ctx, &box3);
  assertEqualsPtr(ctx.root->rightMost, &box3);

  PopParentWidget(&ctx);
  assertEqualsPtr(ctx.root, &column);

  PushParentWidget(&ctx, &row2);
  assertEqualsPtr(ctx.root, &row2);

  // insert children
  box4.sizes[UiAxis_X].kind = UiSizeKind_PERCENTOFPARENT;
  box4.sizes[UiAxis_X].value = 75;
  box4.sizes[UiAxis_X].strictness = 0.666666666666;
  PushChildWidget(&ctx, &box4);
  assertEqualsPtr(box4.parent, &row2);
  assertEqualsPtr(ctx.root->right, &box4);
  assertEqualsPtr(ctx.root->rightMost, &box4);

  box5.sizes[UiAxis_X].kind = UiSizeKind_PERCENTOFPARENT;
  box5.sizes[UiAxis_X].value = 75;
  box5.sizes[UiAxis_X].strictness = 0.666666666666;
  PushChildWidget(&ctx, &box5);
  assertEqualsPtr(ctx.root->rightMost, &box5);
  assertEqualsPtr(&box4, box5.prevSibling);
  assertEqualsPtr(&box5, box4.nextSibling);

  PopParentWidget(&ctx);

  printf("WIDGET SUCCESS\n");

  GUIEnd(&ctx);

  UiWidget *cur = ctx.root->right;

  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 100);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->right;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 40);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 40);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 40);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 20);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 60);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 60);
  assertEqualsFloat(cur->screenRect.y, 0);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 40);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->parent->nextSibling;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 50);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 100);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->right;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 0);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 0);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 50);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  cur = cur->nextSibling;
  assertEqualsFloat(cur->relativePosition[UiAxis_X], 50);
  assertEqualsFloat(cur->relativePosition[UiAxis_Y], 0);
  assertEqualsFloat(cur->screenRect.x, 50);
  assertEqualsFloat(cur->screenRect.y, 50);
  assertEqualsFloat(cur->dimensions[UiAxis_X], 50);
  assertEqualsFloat(cur->dimensions[UiAxis_Y], 50);

  printf("EndDrawing SUCCESS\n");
}
