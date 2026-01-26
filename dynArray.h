/**
 *
 * THIS FILE I VERY MUCH INSPIRED BY TSODING'S nob.h DYNAMIC ARRAY
 * IMPLEMENTATION. A BIG SHOUTOUT TO MISTER ZOZIN, FOR ALL HIS GREAT CONTENT!
 *
 */

#ifndef CORE_DYN_ARRAY_H
#define CORE_DYN_ARRAY_H

#ifndef CORE_ARRAY_REALLOC
#include "allocator.h"
#define CORE_ARRAY_REALLOC ReallocMemory

#endif // !CORE_ARRAY_REALLOC

#ifndef CORE_ARRAY_INIT_CAP
#define CORE_ARRAY_INIT_CAP 256
#endif

#ifdef __cplusplus
#define CORE_ARRAY_DECLTYPE_CAST(T) (decltype(T))
#else
#define CORE_ARRAY_DECLTYPE_CAST(T)
#endif // __cplusplus

#define CORE_ArrayReserve(da, expected_capacity)                               \
  do {                                                                         \
    if ((expected_capacity) > (da)->capacity) {                                \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = CORE_ARRAY_INIT_CAP;                                  \
      }                                                                        \
      while ((expected_capacity) > (da)->capacity) {                           \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items = CORE_ARRAY_DECLTYPE_CAST((da)->items) CORE_ARRAY_REALLOC(  \
          (da)->items, (da)->capacity * sizeof(*(da)->items));                 \
    }                                                                          \
  } while (0)

// Append an item to a dynamic array
#define CORE_ArrayAppend(da, item)                                             \
  do {                                                                         \
    CORE_ArrayReserve((da), (da)->count + 1);                                  \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#ifdef CORE_DYN_ARRAY_IMPLEMENTATION

// implementations if any

#endif // CORE_DYN_ARRAY_IMPLEMENTATION
#endif // !CORE_DYN_ARRAY_H
