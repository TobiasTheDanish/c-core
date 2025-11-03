#ifndef APPLE_ALLOCATOR_C
#define APPLE_ALLOCATOR_C

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define KB 1024
#define MB (1024 * KB)

int32_t pageSize = 0;
int32_t __GetPageSize() {
  if (pageSize == 0) {
    pageSize = sysconf(_SC_PAGESIZE);
  }
  return pageSize;
}

uint32_t __cur_ptr_list_idx = 0;
intptr_t __ptr_list[MB] = {0};
int32_t __len_list[MB] = {0};

void *__AllocMemory(int32_t bytes) {
  void *res = mmap(0, bytes, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if ((intptr_t)res == -1) {
    printf("Error allocating memory. Errno: %d\n", errno);
    abort();
  }
  __ptr_list[__cur_ptr_list_idx] = (intptr_t)res;
  __len_list[__cur_ptr_list_idx++] = bytes;

  return res;
}

void *__AllocMemoryPage() { return __AllocMemory(__GetPageSize()); }

void *__ReallocMemory(void *orig, int32_t bytes) {
  void *dest = __AllocMemory(bytes);
  memcpy(dest, orig, bytes);
  return dest;
}

int32_t __find_ptr_list_idx(intptr_t mem) {
  for (int32_t i = 0; i < MB; i++) {
    if (__ptr_list[i] == mem) {
      return i;
    }
  }

  return -1;
}

void __FreeMemory(void *mem) {
  int32_t idx = __find_ptr_list_idx((intptr_t)mem);
  int32_t len = __len_list[idx];

  munmap(mem, len);
}

#endif /* ifndef APPLE_ALLOCATOR_C */
