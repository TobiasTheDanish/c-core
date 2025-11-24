#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H
#include "common.h"

void *AllocMemory(int32_t bytes);
void *ReallocMemory(void *orig, int32_t bytes);
void FreeMemory(void *mem);

void ZeroMemory(void *ptr);

#ifdef ALLOCATOR_IMPLEMENTATION

#include "allocator/platform_allocator.h"
#ifdef __unix__
#include "allocator/unix_allocator.c"
#endif // __unix__

void *AllocMemory(int32_t bytes) {
  if (bytes == 0) {
    return 0;
  }

  return __AllocMemory(bytes);
}
void *ReallocMemory(void *orig, int32_t bytes) {
  if (bytes == 0) {
    return orig;
  }

  return __ReallocMemory(orig, bytes);
}
void FreeMemory(void *mem) { __FreeMemory(mem); }
void ZeroMemory(void *ptr) { __ZeroMemory(ptr); }

// #endif // !IMPLEMENTATION_GUARD
#endif // ALLOCATOR_IMPLEMENTATION

#endif // !CORE_ALLOCATOR_H
