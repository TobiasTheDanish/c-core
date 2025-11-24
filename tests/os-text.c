#define ALLOCATOR_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#define CORE_OS_IMPLEMENTATION
#include "../os.h"
#define TEST_IMPLEMENTATION
#include "../testing.h"
#include <stdio.h>

int main() {
  assertTrue(
      OS_LoadFont("/home/tobias/dev/c-core/fonts/UniversCondensed.ttf", 48));

  String str = StringFromCString("abcdefghijklmnopqstuvwxyz");
  int32_t width = OS_TextWidth(str, StringLength(str));

  printf("Hello world text width: %d\n", width);

  OS_Window *w = OS_CreateWindow(1080, 720);

  Bool quit = false;
  while (!quit) {
    OS_Event *events = 0;
    int32_t count = 0;
    OS_PollEvents(w, &events, &count);
    for (int32_t i = 0; i < count; i++) {
      OS_Event e = events[i];
      if (e.kind == OS_EventKind_Quit) {
        quit = true;
      }
    }
    FreeMemory(events);

    OS_DrawText(w, str,
                (V2){
                    .x = 540 - ((float)width / 2.0f),
                    .y = 360,
                },
                NewColor(.b = 0xFF, .g = 0xFF, .r = 0xFF));

    OS_BlitWindow(w);
  }

  FreeMemory(str);

  return 0;
}
