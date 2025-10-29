#define TEST_IMPLEMENTATION
#include "../arena.h"
#include "../testing.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void testAllocFree() {
  Arena arena = {0};
  assertEqualsInt(arena.chunkCount, 0);
  assertEqualsInt(arena.chunkCap, 0);
  assertNull(arena.chunks);
  uint8_t *p = ArenaAlloc(&arena, 12);
  assertEqualsInt(arena.chunkCount, 1);
  assertEqualsInt(arena.chunkCap, 1);
  assertNotNull(arena.chunks);
  assertEqualsInt(arena.chunks[0].size, 12);

  for (int i = 0; i < 12; i++) {
    assertEqualsInt(p[i], 0);
  }

  ArenaFree(&arena);
  assertEqualsInt(arena.chunkCount, 0);
  assertEqualsInt(arena.chunkCap, 1);
  assertNotNull(arena.chunks);
  assertEqualsInt(arena.chunks[0].size, 0);

  printf("testAllocFree: SUCCESS\n");
}
void testAllocRealloc() {
  Arena arena = {0};
  assertEqualsInt(arena.chunkCount, 0);
  assertEqualsInt(arena.chunkCap, 0);
  assertNull(arena.chunks);
  uint8_t *p = ArenaAlloc(&arena, 12);
  assertEqualsInt(arena.chunkCount, 1);
  assertEqualsInt(arena.chunkCap, 1);
  assertNotNull(arena.chunks);
  assertEqualsInt(arena.chunks[0].size, 12);

  for (int i = 0; i < 12; i++) {
    assertEqualsInt(p[i], 0);
    p[i] = i;
  }

  uint8_t *p2 = ArenaRealloc(&arena, p, 16);
  assertEqualsInt(arena.chunkCount, 1);
  assertEqualsInt(arena.chunkCap, 1);
  assertEqualsInt(arena.chunks[0].size, 28);

  for (int i = 0; i < 16; i++) {
    assertEqualsInt(p2[i], (i < 12 ? i : 0));
  }

  printf("testAllocRealloc: SUCCESS\n");
}

int main() {
  testAllocFree();
  testAllocRealloc();
}
