#include "../arena.h"
#include "../allocator/platform_allocator.h"

#include <stdint.h>
#include <string.h>

#ifndef ALLOCATOR_IMPLEMENTATION
#ifdef __linux__
#include "../allocator/linux_allocator.c"
#elif __APPLE__
#include "../allocator/apple_allocator.c"
#endif /* ifdef LINUX  */
#endif /* #ifndef ALLOCATOR_IMPLEMENTATION */

void ZeroMemory(void *mem, int32_t bytes) {
  uint8_t *m = mem;
  for (int i = 0; i < bytes; i++) {
    m[i] = 0;
  }
}

void ArenaAllocChunk(Arena *a) {
  if (a->chunkCap > a->chunkCount) {
    a->chunkCount += 1;
    return;
  }

  int32_t idx = a->chunkCount;
  a->chunkCount += 1;
  a->chunkCap += 1;

  a->chunks = __AllocMemory(a->chunkCount * sizeof(Chunk));
  a->chunks[idx].cap = __GetPageSize();
  a->chunks[idx].size = 0;
  a->chunks[idx].data = __AllocMemoryPage();
}

void ArenaInit(Arena *a) {
  a->chunkCount = 0;
  a->chunkCap = 0;
  ArenaAllocChunk(a);
}

void *ArenaAlloc(Arena *a, int32_t bytes) {
  if (a->chunks == 0) {
    ArenaInit(a);
  }
  Chunk curChunk = a->chunks[a->chunkCount - 1];
  if (curChunk.size + bytes >= curChunk.cap) {
    ArenaAllocChunk(a);
    curChunk = a->chunks[a->chunkCount - 1];
  }

  void *res = curChunk.data + curChunk.size;
  ZeroMemory(res, bytes);
  a->chunks[a->chunkCount - 1].size += bytes;
  return res;
}

void *ArenaRealloc(Arena *a, void *orig, int32_t bytes) {
  void *dest = ArenaAlloc(a, bytes);
  memcpy(dest, orig, bytes);

  return dest;
}

void ArenaFree(Arena *a) {
  for (int i = 0; i < a->chunkCount; i++) {
    a->chunks[i].size = 0;
  }

  a->chunkCount = 0;
}
