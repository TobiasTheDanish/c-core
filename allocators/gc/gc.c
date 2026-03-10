#include "../gc.h"
#include "../baseAllocator.h"
#include <stdio.h>

void GC_Free(uintptr_t *ptr);
void __ZeroMemory(void *mem);

void *AllocMemory(int32_t bytes) {
  if (bytes == 0) {
    return 0;
  }

  return CORE_GCAlloc(bytes);
}
void *ReallocMemory(void *orig, int32_t bytes) {
  if (bytes == 0) {
    return orig;
  }

  return CORE_GCRealloc(orig, bytes);
}
void FreeMemory(void *mem) {
  printf("[GC]: User tried to free %p via FreeMemory. I will not do it, call "
         "CORE_GCCollect to free allocated data.\n",
         mem);
}
void ZeroMemory(void *ptr) { __ZeroMemory(ptr); }

#define MEMORY_CAP (128 * 1024 * 1024)
#define CHUNK_SIZE (sizeof(uintptr_t))
#define CHUNK_NUM (MEMORY_CAP / CHUNK_SIZE)

typedef struct {
  uintptr_t *start;
  uint64_t size;
} MemoryChunk;

#define MemoryChunk_end(chunk)                                                 \
  (uintptr_t *)(((uint8_t *)(chunk).start) + (chunk).size)

typedef struct {
  uint64_t count;
  MemoryChunk chunks[CHUNK_NUM];
} ChunkList;

uintptr_t memory[MEMORY_CAP] = {0};

ChunkList freeChunks = {
    .count = 1, .chunks = {[0] = {.start = memory, .size = MEMORY_CAP}}};

ChunkList allocatedChunks = {.count = 0};

void MoveMemory(void *dest, const void *src, uint64_t n) {
  uint8_t *d = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)src;

  for (uint64_t i = 0; i < n; i++) {
    d[i] = s[i];
  }
}

int32_t RoundFloat(float num) {
  int32_t res = num * 10;
  int32_t remainder = res % 10;
  return (res / 10) + (remainder >= 5 ? 1 : 0);
}

uint64_t ChunkList_Insert(ChunkList *list, uintptr_t *start, uint64_t size) {
  list->chunks[list->count].start = start;
  list->chunks[list->count].size = size;

  uint64_t i = list->count;
  for (; i > 0 && list->chunks[i - 1].start > list->chunks[i].start; i--) {
    const MemoryChunk tmp = {.start = list->chunks[i].start,
                             .size = list->chunks[i].size};

    list->chunks[i].start = list->chunks[i - 1].start;
    list->chunks[i].size = list->chunks[i - 1].size;

    list->chunks[i - 1].start = tmp.start;
    list->chunks[i - 1].size = tmp.size;
  }

  list->count += 1;

  return i;
}

void ChunkList_Remove(ChunkList *list, uint64_t index) {
  while (index < list->count - 1) {
    MoveMemory(&list->chunks[index], &list->chunks[index + 1],
               sizeof(MemoryChunk));
    index += 1;
  }
  list->count -= 1;
}

int64_t ChunkList_BinarySearch(const ChunkList *list, uintptr_t *ptr) {
  if (list->count == 0 || ptr == null) {
    return -1;
  }

  printf("ChunkList_BinarySearch - searching for %p\n", ptr);

  uint64_t min = 0;
  uint64_t max = list->count - 1;
  int64_t diff = max - min;
  int64_t index = min + (diff > 0 ? (diff / 2) : 0);

  while ((index >= 0 && index < list->count) && min <= max) {
    if (list->chunks[index].start == ptr) {
      printf("ChunkList_BinarySearch - found %p at idx: %ld\n", ptr, index);
      return index;
    } else if (list->chunks[index].start > ptr) {
      max = index;
    } else if (list->chunks[index].start < ptr) {
      min = index;
    }

    diff = max - min;
    if (diff == 0) {
      break;
    }

    index = min + (diff == 1 ? diff : (diff / 2));
    printf("ChunkList_BinarySearch - diff: %ld, new index: %ld\n", diff, index);
  }

  printf("ChunkList_BinarySearch - did not find %p\n", ptr);
  return -1;
}

void ChunkList_TryMerge(ChunkList *list, uint64_t idx) {
  if (idx >= list->count) {
    return;
  }

  if (idx > 0) {
    const MemoryChunk current = list->chunks[idx];
    const MemoryChunk prev = list->chunks[idx - 1];
    if (MemoryChunk_end(prev) == current.start) {
      list->chunks[idx - 1].size += prev.size;
      ChunkList_Remove(list, idx);
      idx -= 1;
    }
  }

  if (idx < list->count - 1) {
    const MemoryChunk current = list->chunks[idx];
    const MemoryChunk next = list->chunks[idx + 1];
    if (MemoryChunk_end(current) == next.start) {
      list->chunks[idx].size += next.size;
      ChunkList_Remove(list, idx + 1);
    }
  }
}

void *CORE_GCAlloc(uint64_t bytes) {
  if (bytes == 0) {
    return null;
  }

  uint64_t chunksToAllocate = bytes / CHUNK_SIZE;
  if (bytes % CHUNK_SIZE != 0) {
    chunksToAllocate += 1;
  }

  uint64_t allocationSize = chunksToAllocate * CHUNK_SIZE;
  for (int i = 0; i < freeChunks.count; i++) {
    MemoryChunk current = freeChunks.chunks[i];
    if (current.size >= allocationSize) {
      uintptr_t *ptr = current.start;
      ChunkList_Insert(&allocatedChunks, ptr, allocationSize);

      if (current.size > allocationSize) {
        freeChunks.chunks[i].start += chunksToAllocate;
        freeChunks.chunks[i].size -= allocationSize;
      } else {
        ChunkList_Remove(&freeChunks, i);
      }

      return ptr;
    }
  }

  return null;
}

void *CORE_GCRealloc(void *orig, uint64_t bytes) {
  if (bytes == 0 && orig != 0) {
    GC_Free(orig);
    return 0;
  }

  void *dest = CORE_GCAlloc(bytes);

  if (orig) {
    uintptr_t *src = (uintptr_t *)orig;
    int64_t idx = ChunkList_BinarySearch(&allocatedChunks, src);
    if (idx == -1 || allocatedChunks.chunks[idx].start != src) {
      return dest;
    }

    MemoryChunk srcChunk = allocatedChunks.chunks[idx];
    uint64_t bytesToMove = srcChunk.size <= bytes ? srcChunk.size : bytes;

    MoveMemory(dest, orig, bytesToMove);
    GC_Free(orig);
  }

  return dest;
}

void GC_Free(uintptr_t *ptr) {
  if (ptr == null || ptr < memory || (memory + MEMORY_CAP) < ptr) {
    return;
  }

  printf("GC_Free - freeing: %p\n", ptr);

  int64_t idx = ChunkList_BinarySearch(&allocatedChunks, ptr);
  if (idx == -1 || allocatedChunks.chunks[idx].start != ptr) {
    return;
  }

  MemoryChunk toFree = allocatedChunks.chunks[idx];
  uint64_t insertIdx = ChunkList_Insert(&freeChunks, toFree.start, toFree.size);
  ChunkList_TryMerge(&freeChunks, insertIdx);

  ChunkList_Remove(&allocatedChunks, idx);
}

void GC_MarkRegion(uintptr_t **start, uintptr_t **end, Bool *reachableChunks) {
  for (uintptr_t **p = start; p < end; p++) {
    printf("GC_MarkRegion - start: %p, end: %p, current: %p\n", start, end, p);
    uintptr_t *ptr = *p;
    if (ptr == null || ptr < memory || ptr >= memory + MEMORY_CAP) {
      continue;
    }

    printf("GC_MarkRegion - ptr: %p\n", ptr);

    for (uint64_t i = 0; i < allocatedChunks.count; i++) {
      MemoryChunk chunk = allocatedChunks.chunks[i];
      uintptr_t *chunkEnd =
          (uintptr_t *)(((uint8_t *)chunk.start) + chunk.size);
      if (chunk.start <= ptr && ptr < chunkEnd) {
        if (!reachableChunks[i]) {
          printf("GC_MarkRegion - chunk #%zu is reachable\n", i + 1);
          reachableChunks[i] = true;
          GC_MarkRegion(&chunk.start, &chunkEnd, reachableChunks);
        }
      }
    }
  }
}

void __CORE__GCCollect(uintptr_t **start, uintptr_t **end) {
  printf("GC_Collect - start: %p, end: %p\n", start, end);

  Bool reachableChunks[CHUNK_NUM] = {0};
  if (start > end) {
    uintptr_t **tmp = start;
    start = end;
    end = tmp;
  }
  GC_MarkRegion(start, end, reachableChunks);

  ChunkList toFree = {0};
  for (uint64_t i = 0; i < allocatedChunks.count; i++) {
    if (!reachableChunks[i]) {
      toFree.chunks[toFree.count++] = allocatedChunks.chunks[i];
    }
  }

  printf("GC_Collect - chunks to free: %zu\n", toFree.count);

  for (uint64_t i = 0; i < toFree.count; i++) {
    GC_Free(toFree.chunks[i].start);
  }
}

void CORE__GCCollect(uintptr_t **start) {
  __CORE__GCCollect(start, __builtin_frame_address(0));
}

void ChunkList_Dump(ChunkList *list) {
  printf("Chunks: %zu\n", list->count);
  for (uint64_t i = 0; i < list->count; i++) {
    const MemoryChunk current = list->chunks[i];
    printf("#%zu: start: %p, size: %zu\n", i + 1, current.start, current.size);
  }
}

void CORE_GCDumpInfo() {
  printf("Alloced ");
  ChunkList_Dump(&allocatedChunks);
  printf("Freed ");
  ChunkList_Dump(&freeChunks);
}

void __ZeroMemory(void *mem) {
  if (mem == 0) {
    return;
  }

  int64_t idx = ChunkList_BinarySearch(&allocatedChunks, (uintptr_t *)mem);
  if (idx == -1 || allocatedChunks.chunks[idx].start != mem) {
    return;
  }

  MemoryChunk meta = allocatedChunks.chunks[idx];
  uint8_t *bytes = (uint8_t *)mem;
  for (int i = 0; i < meta.size; i++) {
    bytes[i] = 0;
  }
}
