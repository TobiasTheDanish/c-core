#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H
#include "common.h"

void *AllocMemory(int32_t bytes);
void *ReallocMemory(void *orig, int32_t bytes);
void FreeMemory(void *mem);

#ifdef ALLOCATOR_IMPLEMENTATION

#include "allocator/platform_allocator.h"
#ifdef __linux__
#include "allocator/linux_allocator.c"
#endif /* ifdef LINUX  */
#ifdef __APPLE__
#include "allocator/apple_allocator.c"
#endif /* ifdef __APPLE__  */

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

// #endif // !IMPLEMENTATION_GUARD
#endif // ALLOCATOR_IMPLEMENTATION

#endif // !CORE_ALLOCATOR_H
