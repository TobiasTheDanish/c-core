#include "arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

Arena staticArena;

int main() {
  assert(staticArena.chunkCount == 0);
  assert(staticArena.chunkCap == 0);
  assert(staticArena.chunks == 0);
  uint8_t *p = ArenaAlloc(&staticArena, 12);
  assert(staticArena.chunkCount == 1);
  assert(staticArena.chunkCap == 1);
  assert(staticArena.chunks != 0);
  assert(staticArena.chunks[0].size == 12);

  for (int i = 0; i < 12; i++) {
    assert(p[i] == 0);
  }

  ArenaFree(&staticArena);
  assert(staticArena.chunkCount == 0);
  assert(staticArena.chunkCap == 1);
  assert(staticArena.chunks != 0);
  assert(staticArena.chunks[0].size == 0);

  printf("SUCCESS\n");
}
