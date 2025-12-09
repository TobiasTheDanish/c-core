#ifndef CORE_ALLOCATOR_IMPLEMENTATION
#define CORE_ALLOCATOR_IMPLEMENTATION
#include <string.h>
#endif /* ifndef CORE_ALLOCATOR_IMPLEMENTATION */
#include "../allocator.h"
#include "platform_testing.h"
#include <stdio.h>

Bool _CompareStrings(char *a, char *b) {
  uint64_t aLen = strlen(a);
  uint64_t bLen = strlen(b);
  if (aLen != bLen) {
    return false;
  }

  for (int i = 0; a[i] && b[i]; i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }

  return true;
}

#ifdef __linux__
#include "linux_testing.c"
#elif __APPLE__
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
  snprintf(__buf, 75, "Actual: %lld. Expected: %lld\n", actual, expected);
  _assert(actual == expected, __buf, file, line);
}
void _assertEqualsUint(uint64_t actual, uint64_t expected, char *file,
                       int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %llu. Expected: %llu\n", actual, expected);
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
void _assertEqualsStr(char *actual, char *expected, char *file, int line) {
  char __buf[150] = {0};
  snprintf(__buf, 150, "Actual: %s. Expected: %s.\n", actual, expected);
  _assert(_CompareStrings(actual, expected), __buf, file, line);
}

void _assertNotEqualsInt(int64_t actual, int64_t expected, char *file,
                         int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %lld. Expected: %lld\n", actual, expected);
  _assert(actual != expected, __buf, file, line);
}
void _assertNotEqualsUint(uint64_t actual, uint64_t expected, char *file,
                          int line) {
  char __buf[75] = {0};
  snprintf(__buf, 75, "Actual: %llu. Expected: %llu\n", actual, expected);
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
void _assertNotEqualsStr(char *actual, char *expected, char *file, int line) {
  char __buf[150] = {0};
  snprintf(__buf, 150, "Actual: %s. Expected: %s\n", actual, expected);
  _assert(!_CompareStrings(actual, expected), __buf, file, line);
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
  _assertEqualsInt((actual), (expected), __FILE__, __LINE__)
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
#define assertEqualsStr(actual, expected)                                      \
  _assertEqualsStr((actual), (expected), __FILE__, __LINE__)
#define assertTrue(actual)                                                     \
  _assert((actual), "Expected true, but was false\n", __FILE__, __LINE__)

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
#define assertNotEqualsStr(actual, expected)                                   \
  _assertNotEqualsStr((actual), (expected), __FILE__, __LINE__)

#define assertFalse(actual)                                                    \
  _assert(!(actual), "Expected false, but was true\n", __FILE__, __LINE__)
