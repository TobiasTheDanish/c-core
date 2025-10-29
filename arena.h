#ifndef ARENA_H
#define ARENA_H
#include <stdint.h>

typedef struct {
  int32_t size;
  int32_t cap;
  void *data;
} Chunk;

typedef struct {
  Chunk *chunks;
  int64_t chunkCount;
  int64_t chunkCap;
} Arena;

void *ArenaAlloc(Arena *a, int32_t bytes);
void *ArenaRealloc(Arena *a, void *orig, int32_t bytes);
void ArenaFree(Arena *a);
#endif // !ARENA_H
