#include "../arena.h"
#define CORE_ALLOCATOR_IMPLEMENTATION
#include "../allocator.h"

#include <stdint.h>
#include <string.h>

void _ZeroMemory(uint8_t *mem, int32_t bytes) {
  for (int i = 0; i < bytes; i++) {
    mem[i] = 0;
  }
}

void ArenaAllocChunk(Arena *a, int32_t minCap) {
  if (a->chunkCap > a->chunkCount && a->chunks[a->chunkCount].cap > minCap) {
    a->chunkCount += 1;
    return;
  }

  int32_t idx = a->chunkCount;
  if (a->chunkCap == a->chunkCount) {
    a->chunkCap += 1;
  }
  a->chunkCount += 1;

  int32_t pageSize = __GetPageSize();
  int32_t chunkCap =
      minCap < pageSize ? pageSize : ((minCap / pageSize) + 1) * pageSize;

  a->chunks = __ReallocMemory(a->chunks, a->chunkCount * sizeof(Chunk));
  a->chunks[idx].cap = chunkCap;
  a->chunks[idx].size = 0;
  a->chunks[idx].data = __AllocMemory(chunkCap);
}

void ArenaInit(Arena *a) {
  a->chunkCount = 0;
  a->chunkCap = 0;
  ArenaAllocChunk(a, 0);
}

void *ArenaAlloc(Arena *a, int32_t bytes) {
  if (a->chunks == 0) {
    ArenaInit(a);
  }
  Chunk curChunk = a->chunks[a->chunkCount - 1];
  if (curChunk.size + bytes >= curChunk.cap) {
    ArenaAllocChunk(a, bytes);
    curChunk = a->chunks[a->chunkCount - 1];
  }

  uint8_t *res = curChunk.data;
  res += curChunk.size;
  _ZeroMemory(res, bytes);
  a->chunks[a->chunkCount - 1].size += bytes;
  return (void *)res;
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
