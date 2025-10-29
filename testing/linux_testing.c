#include "platform_testing.h"
#include <unistd.h>

void platformAssert(Bool test, char *msg) {
  int32_t len = 0;
  while (msg[len++]) {
  }

  if (!test) {
    write(STDOUT_FILENO, msg, len);
    fsync(STDOUT_FILENO);
    _exit(1);
  }
}
