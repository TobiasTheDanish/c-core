#ifndef CORE_OS_FILES_C
#define CORE_OS_FILES_C
#include "../os.h"
#include <stdio.h>

String OS_ReadEntireFile(String path) {
  FILE *f = fopen(path, "r");

  String content = StringEmpty();
  char buf[1024] = {0};
  uint64_t charsRead = 1024;

  while (charsRead == 1024) {
    charsRead = fread(buf, 1, 1024, f);
    content = StringAddChars(content, buf, charsRead);
  }

  return content;
}

String OS_ReadEntireFileFromPath(Path path) {
  return OS_ReadEntireFile(Path_ToString(path));
}
#endif /* ifndef CORE_OS_FILES_C */
