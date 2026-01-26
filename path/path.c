#define CORE_ALLOCATOR_IMPLEMENTATION
#define CORE_STRING_IMPLEMENTATION
#include "../path.h"

#define DELIMITER '/'

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
