#include <stdio.h>
#define ARENA_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#include "../string.h"
#define TEST_IMPLEMENTATION
#include "../testing.h"

int main() {
  String empty = StringEmpty();
  assertEqualsInt(StringLength(empty), 0);
  assertEqualsInt(StringCap(empty), 4);

  String world = StringAdd(empty, ' ');
  assertEqualsPtr(world, empty);
  assertEqualsInt(StringLength(world), 1);
  assertEqualsInt(world[0], ' ');

  world = StringAddAll(world, "World!\n");
  assertEqualsInt(StringLength(world), 8);
  assertEqualsInt(StringCap(world), 8);
  assertEqualsInt(world[0], ' ');
  char *expectedWorld = " World!\n";
  for (int i = 0; i < StringLength(world); i++) {
    assertEqualsChar(world[i], expectedWorld[i]);
  }

  String hello = StringFromCString("Hello");
  assertEqualsInt(StringLength(hello), 5);
  assertEqualsInt(StringCap(hello), 10);
  char *expectedHello = "Hello";
  for (int i = 0; i < StringLength(hello); i++) {
    assertEqualsChar(hello[i], expectedHello[i]);
  }

  hello = StringConcat(hello, world);
  assertEqualsInt(StringLength(hello), 13);
  assertEqualsInt(StringCap(hello), 20);

  char *helloWorld = "Hello World!\n";
  for (int i = 0; i < StringLength(hello); i++) {
    assertEqualsInt(hello[i], helloWorld[i]);
  }

  printf("SUCCESS\n");
}
