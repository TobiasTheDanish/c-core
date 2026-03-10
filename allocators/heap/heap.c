#include "../baseAllocator.h"

#include "platform_allocator.h"
#ifdef __unix__
#include "unix_allocator.c"
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
