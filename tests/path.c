#include "../path.h"
#include <stdio.h>
#define CORE_TEST_IMPLEMENTATION
#include "../testing.h"

void testPathFromString() {
  String pathString =
      StringFromCString("../thirdparty/RGFW-1.70_linux_amd64/README.md");

  Path p = Path_FromString(pathString);

  assertTrue(p.isRelative);
  assertEqualsInt(p.elements.count, 4);
  assertEqualsStr(p.elements.items[0], "..");
  assertEqualsStr(p.elements.items[1], "thirdparty");
  assertEqualsStr(p.elements.items[2], "RGFW-1.70_linux_amd64");
  assertEqualsStr(p.elements.items[3], "README.md");

  pathString = StringFromCString("/usr/lib/libc.a");

  p = Path_FromString(pathString);

  assertFalse(p.isRelative);
  assertEqualsInt(p.elements.count, 3);
  assertEqualsStr(p.elements.items[0], "usr");
  assertEqualsStr(p.elements.items[1], "lib");
  assertEqualsStr(p.elements.items[2], "libc.a");

  pathString = StringFromCString("usr/lib/libc.a");

  p = Path_FromString(pathString);

  assertTrue(p.isRelative);
  assertEqualsInt(p.elements.count, 3);
  assertEqualsStr(p.elements.items[0], "usr");
  assertEqualsStr(p.elements.items[1], "lib");
  assertEqualsStr(p.elements.items[2], "libc.a");
}

int main() {
  testPathFromString();
  printf("testPathFromString successful\n");

  return 0;
}
