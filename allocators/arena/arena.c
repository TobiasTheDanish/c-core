#include "../arena.h"
#include "../baseAllocator.h"
#include "../heap/platform_allocator.h"
#include <stdint.h>
#include <string.h>

#ifdef __unix__
#include "../heap/unix_allocator.c"
#endif // __unix__

typedef struct {
  int64_t size;
} AllocMetaData;

#define ARENA_STACK_LIMIT 128

Arena defaultArena = {0};

Arena *arenaStack[ARENA_STACK_LIMIT];
int32_t currentArenaIdx = -1;
#define currentArena (arenaStack[currentArenaIdx])

void ArenaPushCurrent(Arena *a) {
  currentArenaIdx++;
  arenaStack[currentArenaIdx] = a;
}

void ArenaPopCurrent() {
  arenaStack[currentArenaIdx] = null;
  currentArenaIdx--;
}

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
  if (a->chunks == 0 || a->chunkCount == 0) {
    ArenaInit(a);
  }
  int64_t allocationSize = bytes + sizeof(AllocMetaData);

  Chunk curChunk = a->chunks[a->chunkCount - 1];
  if (curChunk.size + allocationSize >= curChunk.cap) {
    ArenaAllocChunk(a, allocationSize);
    curChunk = a->chunks[a->chunkCount - 1];
  }

  uint8_t *res = curChunk.data;
  res += curChunk.size;
  a->chunks[a->chunkCount - 1].size += allocationSize;
  _ZeroMemory(res, allocationSize);

  AllocMetaData *meta = (AllocMetaData *)res;
  meta->size = bytes;
  meta += 1;

  return (void *)meta;
}

void *ArenaRealloc(Arena *a, void *orig, int32_t bytes) {
  if (bytes == 0) {
    return 0;
  }
  if (a->chunks == 0 || a->chunkCount == 0) {
    ArenaInit(a);
  }

  void *dest = ArenaAlloc(a, bytes);

  if (orig != 0) {
    AllocMetaData *origMeta = ((AllocMetaData *)orig) - 1;

    if (origMeta->size <= bytes) {
      memcpy(dest, orig, origMeta->size);
    } else {
      memcpy(dest, orig, bytes);
    }
  }

  return dest;
}

void ArenaFree(Arena *a) {
  for (int i = 0; i < a->chunkCount; i++) {
    a->chunks[i].size = 0;
  }

  a->chunkCount = 0;
}

void *AllocMemory(int32_t bytes) {
  if (bytes == 0) {
    return 0;
  }
  if (currentArenaIdx == -1 || currentArena == null) {
    ArenaPushCurrent(&defaultArena);
  }

  return ArenaAlloc(currentArena, bytes);
}
void *ReallocMemory(void *orig, int32_t bytes) {
  if (bytes == 0) {
    return orig;
  }
  if (currentArenaIdx == -1 || currentArena == null) {
    ArenaPushCurrent(&defaultArena);
  }

  return ArenaRealloc(currentArena, orig, bytes);
}
void FreeMemory(void *mem) {
  // This is just a stub function
}
void ZeroMemory(void *ptr) {
  AllocMetaData *meta = ((AllocMetaData *)ptr) - 1;

  _ZeroMemory(ptr, meta->size);
}
