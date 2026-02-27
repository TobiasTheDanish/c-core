#include <stdlib.h>
#include <unistd.h>

// #define CORE_ALLOCATOR_IMPLEMENTATION
// #define CORE_STRING_IMPLEMENTATION
#include "../path.h"

#define DELIMITER '/'

void Path__Free(Path *p) {
  for (int32_t i = 0; i < p->elements.count; i++) {
    StringFree(p->elements.items[i]);
  }
  FreeMemory(p->elements.items);
  p->elements.items = null;
}

Path Path_FromString(String p) {
  Path path = {0};

  int32_t pLen = StringLength(p);

  if (pLen > 0) {
    path.isRelative = p[0] != DELIMITER;

    int32_t start = 0;
    while (start < pLen) {
      int32_t next = StringFindChar(p, DELIMITER, start);
      if (next == -1) {
        next = pLen;
      }

      String element = StringSubStr(p, start, next);
      if (StringLength(element) > 0) {
        CORE_ArrayAppend(&path.elements, element);
      } else {
        StringFree(element);
      }

      start = next + 1;
    }
  }

  return path;
}

String Path_ToString(Path p) {
  Path absolutePath;
  if (p.isRelative) {
    absolutePath = Path_WorkDir();
    Path_Concat(&absolutePath, p);
  } else {
    absolutePath = p;
  }

  String res = StringEmpty();
  for (int32_t i = 0; i < absolutePath.elements.count; i++) {
    res = StringAdd(res, '/');
    String e = StringCopy(absolutePath.elements.items[i]);
    res = StringAddChars(res, e, StringLength(e));
    StringFree(e);
  }
  Path__Free(&absolutePath);

  return res;
}

void Path_Concat(Path *dst, Path src) {
  for (int32_t i = 0; i < src.elements.count; i++) {
    CORE_ArrayAppend(&dst->elements, src.elements.items[i]);
  }
}

Path Path_WorkDir() {
  char *pathCStr = getcwd(0, 0);
  String pathStr = StringFromCString(pathCStr);
  free(pathCStr);
  return Path_FromString(pathStr);
}
