#include "../arena.h"
#include "../baseAllocator.h"
#include "../heap/platform_allocator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include "../heap/unix_allocator.c"
#endif // __unix__

#define ARENA_STACK_LIMIT 128
#define _64GB ((int64_t)64 * 1024 * 1024 * 1024)

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

// void ArenaAllocChunk(Arena *a, int32_t minCap) {
//   if (a->chunkCap > a->chunkCount && a->chunks[a->chunkCount].cap > minCap) {
//     a->chunkCount += 1;
//     return;
//   }
//
//   int32_t idx = a->chunkCount;
//   if (a->chunkCap == a->chunkCount) {
//     a->chunkCap += 1;
//   }
//   a->chunkCount += 1;
//
//   int32_t pageSize = __GetPageSize();
//   int32_t chunkCap =
//       minCap < pageSize ? pageSize : ((minCap / pageSize) + 1) * pageSize;
//
//   a->chunks = __ReallocMemory(a->chunks, a->chunkCount * sizeof(Chunk));
//   a->chunks[idx].cap = chunkCap;
//   a->chunks[idx].size = 0;
//   a->chunks[idx].data = __AllocMemory(chunkCap);
// }

void ArenaAllocChunk(Arena *a, int32_t minCap) {
  int64_t available = a->committed - a->used;
  if (available >= minCap)
    return;

  int32_t pageSize = __GetPageSize();
  int32_t chunkCap = (minCap - available) < pageSize
                         ? pageSize
                         : ((minCap / pageSize) + 1) * pageSize;

  void *committedEnd = (void *)(((uint8_t *)a->data) + a->committed);
  if (!__CommitMemory(committedEnd, chunkCap)) {
    printf("COULD NOT COMMIT MEMORY OF SIZE %d\n", chunkCap);
    exit(1);
  }
  a->committed += chunkCap;
  void *usedEnd = (void *)(((uint8_t *)a->data) + a->used + 1);
  _ZeroMemory(usedEnd, (a->committed - a->used - 1));
}

void ArenaInit(Arena *a) {
  a->used = 0;
  a->committed = 0;
  a->reserved = _64GB;
  a->data = __ReserveMemory(_64GB);
  ArenaAllocChunk(a, 0);
}

#define NodeIsValidRange(a, n)                                                 \
  ((a)->data <= ((void *)n) &&                                                 \
   ((uint8_t *)n) < (((uint8_t *)(a)->data) + (a)->committed))

void *ArenaSearchFreeList(Arena *a, int32_t bytes) {
  for (AllocMetaData *node = a->freeList;
       node != null && NodeIsValidRange(a, node); node = node->nextFree) {
    if (bytes <= node->size && node->size <= bytes + 16) {
      if (node->nextFree && NodeIsValidRange(a, node->nextFree)) {
        node->nextFree->prevFree = node->prevFree;
      } else {
        node->nextFree = null;
      }

      if (node->prevFree && NodeIsValidRange(a, node->prevFree)) {
        node->prevFree->nextFree = node->nextFree;
      } else if (node->prevFree == null) {
        a->freeList = node->nextFree;
      }
      node->nextFree = null;
      node->prevFree = null;
      if (node == a->freeList) {
        a->freeList = null;
      }
      void *res = (void *)(node + 1);
      __ZeroMemory(res);
      return res;
    }
  }

  return null;
}

void *ArenaAlloc(Arena *a, int32_t bytes) {
  if (a->data == 0) {
    ArenaInit(a);
  }
  void *freeListItem = ArenaSearchFreeList(a, bytes);
  if (freeListItem != null) {
    return freeListItem;
  }

  int64_t allocationSize = bytes + sizeof(AllocMetaData);

  if (a->used + allocationSize >= a->committed) {
    ArenaAllocChunk(a, allocationSize);
  }

  uint8_t *res = (uint8_t *)a->data;
  res += a->used;
  a->used += allocationSize;
  _ZeroMemory(res, allocationSize);

  AllocMetaData *meta = (AllocMetaData *)res;
  meta->size = bytes;
  meta->nextFree = null;
  meta->prevFree = null;
  meta += 1;

  return (void *)meta;
}

void *ArenaRealloc(Arena *a, void *orig, int32_t bytes) {
  if (bytes == 0) {
    FreeMemory(orig);
    return 0;
  }
  if (a->data == 0) {
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
  FreeMemory(orig);

  return dest;
}

void ArenaFree(Arena *a) {
  a->freeList = 0;
  if (__ReleaseMemory(a->data, a->committed)) {
    a->committed = 0;
  }

  a->used = 0;
}

void *ArenaCopyTo(Arena *a, void *ptr, int32_t size) {
  if (ptr == null || size == 0) {
    return null;
  }
  uint8_t *src = (uint8_t *)ptr;
  uint8_t *dst = ArenaAlloc(a, size);

  for (int32_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }

  return (void *)dst;
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
  if (mem == null) {
    return;
  }
  if (currentArenaIdx == -1 || currentArena == null) {
    ArenaPushCurrent(&defaultArena);
  }

  AllocMetaData *meta = ((AllocMetaData *)mem) - 1;
  meta->nextFree = currentArena->freeList;

  if (currentArena->freeList &&
      NodeIsValidRange(currentArena, currentArena->freeList)) {
    currentArena->freeList->prevFree = meta;
  }

  currentArena->freeList = meta;
}

void ZeroMemory(void *ptr) {
  AllocMetaData *meta = ((AllocMetaData *)ptr) - 1;

  _ZeroMemory(ptr, meta->size);
}
