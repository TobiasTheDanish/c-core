#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H
#include "types.h"

void *AllocMemory(int32_t bytes);
void *ReallocMemory(void *orig, int32_t bytes);
void FreeMemory(void *mem);

#endif // !CORE_ALLOCATOR_H

#define ALLOCATOR_IMPLEMENTATION
#ifdef ALLOCATOR_IMPLEMENTATION
#ifndef IMPLEMENTATION_GUARD
#define IMPLEMENTATION_GUARD

#include "allocator/platform_allocator.h"
#ifdef __linux__
#include "allocator/linux_allocator.c"
#endif /* ifdef LINUX  */
#ifdef WINDOWS
// include windows specific arena code
#endif /* ifdef WINDOWS  */

void *AllocMemory(int32_t bytes) { return __AllocMemory(bytes); }
void *ReallocMemory(void *orig, int32_t bytes) { __ReallocMemory(orig, bytes); }
void FreeMemory(void *mem) { __FreeMemory(mem); }

#endif // !IMPLEMENTATION_GUARD
#endif // ALLOCATOR_IMPLEMENTATION
