#include <stdint.h>
int32_t __GetPageSize();

void *__AllocMemory(int32_t bytes);
void *__AllocMemoryPage();

void *__ReallocMemory(void *orig, int32_t bytes);

void __FreeMemory(void *mem);
