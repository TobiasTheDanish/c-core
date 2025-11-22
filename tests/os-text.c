#include "../os.h"
#include "../os/text.c"
#define TEST_IMPLEMENTATION
#include "../testing.h"
#include <stdio.h>

int main() {
  assertTrue(
      loadFont("/home/tobias/dev/c-core/fonts/UniversCondensed.ttf", 24));

  int32_t width = textWidth("Hello world", 11);

  printf("Hello world text width: %d\n", width);

  return 0;
}
