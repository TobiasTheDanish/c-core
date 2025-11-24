#include <stddef.h>
#include <stdint.h>
int32_t __GetPageSize();

void *__AllocMemory(size_t bytes);
void *__AllocMemoryPage();

void *__ReallocMemory(void *orig, size_t bytes);

void __FreeMemory(void *mem);
void __ZeroMemory(void *mem);
