#include "../allocator.h"
#include "platform_testing.h"
#include <stdio.h>

#ifdef __linux__
#include "linux_testing.c"
#endif /* ifdef __linux__ */

void _assert(Bool test, char *msg, char *file, int line) {
  int32_t fileLen = 0;
  while (file[fileLen++]) {
  }
  int32_t msgLen = 0;
  while (msg[msgLen++]) {
  }

  int32_t bufSize = fileLen + msgLen + 30;
  char *buf = AllocMemory(bufSize * sizeof(char));
  snprintf(buf, bufSize, "Assertion failed %s:%d\n\t%s", file, line, msg);
  platformAssert(test, buf);
  FreeMemory(buf);
}

void _assertEqualsInt(int64_t actual, int64_t expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %ld. Expected: %ld\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsUint(uint64_t actual, uint64_t expected, char *file,
                       int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %ld. Expected: %ld\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsFloat(float actual, float expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %f. Expected: %f\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsDouble(double actual, double expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %f. Expected: %f\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsPtr(void *actual, void *expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %p. Expected: %p\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsChar(char actual, char expected, char *file, int line) {
  char __buf[90] = {0};
  snprintf(__buf, 90, "Actual: %c. Expected: %c\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}

void _assertNotEqualsInt(int64_t actual, int64_t expected, char *file,
                         int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %ld. Expected: %ld\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsUint(uint64_t actual, uint64_t expected, char *file,
                          int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %ld. Expected: %ld\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsFloat(float actual, float expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %f. Expected: %f\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsDouble(double actual, double expected, char *file,
                            int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %f. Expected: %f\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsPtr(void *actual, void *expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %p. Expected: %p\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsChar(char actual, char expected, char *file, int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %c. Expected: %c\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}

#define toString(v)                                                            \
  {                                                                            \
    typeof(v);                                                                 \
  }

#define assertNull(p)                                                          \
  {                                                                            \
    _assert(p == 0, "Pointer was not null\n", __FILE__, __LINE__);             \
  }
#define assertNotNull(p)                                                       \
  {                                                                            \
    _assert(p != 0, "Pointer was null\n", __FILE__, __LINE__);                 \
  }

#define assertEqualsInt(actual, expected)                                      \
  _assertEqualsInt(actual, expected, __FILE__, __LINE__)
#define assertEqualsUint(actual, expected)                                     \
  _assertEqualsUint(actual, expected, __FILE__, __LINE__)
#define assertEqualsFloat(actual, expected)                                    \
  _assertEqualsFloat(actual, expected, __FILE__, __LINE__)
#define assertEqualsDouble(actual, expected)                                   \
  _assertEqualsDouble(actual, expected, __FILE__, __LINE__)
#define assertEqualsPtr(actual, expected)                                      \
  _assertEqualsPtr(actual, expected, __FILE__, __LINE__)
#define assertEqualsChar(actual, expected)                                     \
  _assertEqualsChar(actual, expected, __FILE__, __LINE__)

#define assertNotEqualsInt(actual, expected)                                   \
  _assertNotEqualsInt(actual, expected, __FILE__, __LINE__)
#define assertNotEqualsUint(actual, expected)                                  \
  _assertNotEqualsUint(actual, expected, __FILE__, __LINE__)
#define assertNotEqualsFloat(actual, expected)                                 \
  _assertNotEqualsFloat(actual, expected, __FILE__, __LINE__)
#define assertNotEqualsDouble(actual, expected)                                \
  _assertNotEqualsDouble(actual, expected, __FILE__, __LINE__)
#define assertNotEqualsPtr(actual, expected)                                   \
  _assertNotEqualsPtr(actual, expected, __FILE__, __LINE__)
#define assertNotEqualsChar(actual, expected)                                  \
  _assertNotEqualsChar(actual, expected, __FILE__, __LINE__)
