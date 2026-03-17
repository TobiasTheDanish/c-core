#ifndef ARENA_H
#define ARENA_H
#include <stdint.h>

typedef struct AllocMetaData {
  int64_t size;
  struct AllocMetaData *nextFree;
  struct AllocMetaData *prevFree;
} AllocMetaData;

typedef struct {
  int64_t used;
  int64_t committed;
  int64_t reserved;
  void *data;
  AllocMetaData *freeList;
} Arena;

void *ArenaAlloc(Arena *a, int32_t bytes);
void *ArenaRealloc(Arena *a, void *orig, int32_t bytes);
void ArenaFree(Arena *a);
void *ArenaCopyTo(Arena *a, void *ptr, int32_t size);

void ArenaPushCurrent(Arena *a);
void ArenaPopCurrent();

#endif // !ARENA_H
