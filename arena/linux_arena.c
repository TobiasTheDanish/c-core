#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int32_t pageSize = 0;
int32_t __GetPageSize() {
  if (pageSize == 0) {
    pageSize = sysconf(_SC_PAGESIZE);
  }
  return pageSize;
}

void *__AllocMemory(int32_t bytes) {
  void *res = mmap(0, bytes, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if ((intptr_t)res == -1) {
    printf("Error allocating memory. Errno: %d\n", errno);
    abort();
  }

  return res;
}

void *__AllocMemoryPage() { return __AllocMemory(__GetPageSize()); }
