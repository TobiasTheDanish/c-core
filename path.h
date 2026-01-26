#ifndef CORE_PATH_H
#define CORE_PATH_H
#include "common.h"
#include "dynArray.h"
#include "os.h"

typedef struct {
  int32_t count;
  int32_t capacity;
  String *items;
} PathElements;

typedef struct {
  Bool isRelative;
  PathElements elements;
} Path;

Path Path_FromString(String p);

#endif // !CORE_PATH_H
