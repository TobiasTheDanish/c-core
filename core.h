#ifndef CORE_H

#ifdef CORE_IMPLEMENTATION
#define CORE_ALLOCATOR_IMPLEMENTATION
#define CORE_ARENA_IMPLEMENTATION
#define CORE_GUI_IMPLEMENTATION
#define CORE_OS_IMPLEMENTATION
#define CORE_STRING_IMPLEMENTATION
#define CORE_TEST_IMPLEMENTATION
#define CORE_MATH_IMPLEMENTATION
#define CORE_DYN_ARRAY_IMPLEMENTATION
#include "threadpool/threadpool.c"

#endif // CORE_IMPLEMENTATION

#include "allocator.h"
#include "arena.h"
#include "common.h"
#include "dynArray.h"
#include "gui.h"
#include "math.h"
#include "os.h"
#include "string.h"
#include "testing.h"
#include "threadpool.h"

#endif // !CORE_H
