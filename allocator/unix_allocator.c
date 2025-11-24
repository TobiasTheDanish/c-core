#ifndef UNIX_ALLOCATOR_C
#define UNIX_ALLOCATOR_C

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
  size_t size;
} AllocMetadata;

size_t ALLOC_META_SIZE = sizeof(AllocMetadata);

int32_t pageSize = 0;
int32_t __GetPageSize() {
  if (pageSize == 0) {
    pageSize = sysconf(_SC_PAGESIZE);
  }
  return pageSize;
}

void *__AllocMemory(size_t bytes) {
  if (bytes == 0) {
    return 0;
  }

  bytes = (bytes + (sizeof(void *) - 1) & ~(sizeof(void *) - 1));

  void *ptr = mmap(0, bytes + ALLOC_META_SIZE, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (ptr == MAP_FAILED) {
    printf("Error allocating memory. Errno: %d\n", errno);
    return 0;
  }

  AllocMetadata *meta = (AllocMetadata *)ptr;
  meta->size = bytes;

  return (meta + 1);
}

void *__AllocMemoryPage() { return __AllocMemory(__GetPageSize()); }

void *__ReallocMemory(void *orig, size_t bytes) {
  if (bytes == 0) {
    return 0;
  }

  void *dest = __AllocMemory(bytes);

  if (orig) {
    AllocMetadata *origMeta = ((AllocMetadata *)orig) - 1;
    if (origMeta->size <= bytes) {
      memcpy(dest, orig, origMeta->size);
    } else {
      memcpy(dest, orig, bytes);
    }
  }
  return dest;
}

void __FreeMemory(void *mem) {
  if (mem == 0) {
    return;
  }

  AllocMetadata *meta = ((AllocMetadata *)mem) - 1;

  if (munmap(meta, meta->size + ALLOC_META_SIZE) == -1) {
    printf("Error unmapping memory. Errno: %d\n", errno);
  }
}

void __ZeroMemory(void *mem) {
  if (mem == 0) {
    return;
  }

  AllocMetadata *meta = ((AllocMetadata *)mem) - 1;
  uint8_t *bytes = (uint8_t *)mem;
  for (int i = 0; i < meta->size; i++) {
    bytes[i] = 0;
  }
}

#endif /* ifndef UNIX_ALLOCATOR_C */
