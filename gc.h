#ifndef CORE_GC_H
#define CORE_GC_H

#include "common.h"

void *CORE_GCAlloc(uint64_t bytes);
void CORE__GCCollect(uintptr_t **start);

#define CORE_GCCollect() CORE__GCCollect(__builtin_frame_address(0))

void CORE_GCDumpInfo();

#endif // !CORE_GC_H
