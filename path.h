#ifndef CORE_PATH_H
#define CORE_PATH_H
#include "common.h"
#include "dynArray.h"
#include "string.h"

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
String Path_ToString(Path p);

void Path_Concat(Path *dst, Path src);

Path Path_WorkDir();

#endif // !CORE_PATH_H
