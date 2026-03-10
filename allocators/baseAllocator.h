#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H
#include "../common.h"

void *AllocMemory(int32_t bytes);
void *ReallocMemory(void *orig, int32_t bytes);
void FreeMemory(void *mem);

void ZeroMemory(void *ptr);

#endif
