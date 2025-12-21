#ifndef CORE_STRING_H
#define CORE_STRING_H
#include "common.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define StringPush StringAdd

typedef char *String;

String StringEmpty();
String StringFromCString(char *s);
String StringFromCStringL(char *s, int32_t len);

/** Resets the length and reuses the memory of the string */
String StringReset(String s);
void StringFree(String s);

int32_t StringLength(String s);
int32_t StringCap(String s);

String StringAdd(String s, char c);
String StringAddAll(String s, char *c);
String StringAddChars(String s, char *c, int32_t count);
String StringInsert(String s, int32_t index, char c);
String StringConcat(String s, String o);

String StringRepeatChar(char c, int32_t times);

String StringFromInt(int32_t i);
String StringFromFloat(float f);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !CORE_STRING_H

#ifdef CORE_STRING_IMPLEMENTATION
#ifndef STRING_IMPLEMENTATION_GUARD
#define STRING_IMPLEMENTATION_GUARD

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

#define __Alloc(b) AllocMemory(b)
#define __Realloc(p, b) ReallocMemory((p), (b))
#define __Free(p) FreeMemory(p)

#define __AllocString(c) __Alloc((sizeof(char) * (c)) + sizeof(StringMeta))
#define __ReallocString(s, c)                                                  \
  __Realloc(_StringMeta(s), (sizeof(char) * (c)) + sizeof(StringMeta))

typedef struct {
  int32_t length;
  int32_t cap;
} StringMeta;

#define _StringMeta(s) ((StringMeta *)(s) - 1)

/** returns the length of the null terminated string WITHOUT the null character
 */
int32_t CStringLength(char *s) {
  int32_t len = 0;
  while (s[len++])
    ;
  return len - 1;
}

String StringEmpty() { return StringFromCStringL("", 0); }

String StringFromCString(char *s) {
  return StringFromCStringL(s, CStringLength(s));
}

String StringFromCStringL(char *s, int32_t len) {
  int32_t cap = len ? len * 2 : 4;

  String m = (String)__AllocString(cap);

  m = m + sizeof(StringMeta);
  _StringMeta(m)->length = len;
  _StringMeta(m)->cap = cap;

  for (int32_t i = 0; i < len; i++) {
    m[i] = s[i];
  }

  return m;
}

String StringReset(String s) {
  if (s != 0) {
    _StringMeta(s)->length = 0;
  }
  return s;
}

void StringFree(String s) { __Free(_StringMeta(s)); }

int32_t StringLength(String s) {
  StringMeta *m = _StringMeta(s);

  return m->length;
}
int32_t StringCap(String s) {
  StringMeta *m = _StringMeta(s);

  return m->cap;
}

String StringMaybeGrow(String s, int32_t addLen) {
  if (StringLength(s) + addLen >= StringCap(s)) {
    int32_t curLen = StringLength(s);
    int32_t minCap = StringLength(s) + addLen;
    if (minCap < StringCap(s) * 2) {
      minCap = StringCap(s) * 2;
    }
    String m = (String)__ReallocString(s, minCap);

    s = m + sizeof(StringMeta);
    _StringMeta(s)->cap = minCap;
    _StringMeta(s)->length = curLen;
  }
  return s;
}

String StringAdd(String s, char c) {
  s = StringMaybeGrow(s, 1);
  s[_StringMeta(s)->length++] = c;
  return s;
}

String StringAddAll(String s, char *c) {
  int32_t cLen = CStringLength(c);
  if (cLen == 0) {
    return s;
  }

  return StringAddChars(s, c, cLen);
}

String StringAddChars(String s, char *c, int32_t count) {
  s = StringMaybeGrow(s, count);

  for (int32_t i = 0; i < count; i++) {
    s[(_StringMeta(s)->length)++] = c[i];
  }
  return s;
}

String StringInsert(String s, int32_t index, char c) {
  s = StringMaybeGrow(s, 1);

  int32_t len = _StringMeta(s)->length;
  for (int32_t i = len; len >= index; i--) {
    s[i + 1] = s[i];
  }

  s[index] = c;
  _StringMeta(s)->length += 1;

  return s;
}

String StringConcat(String s, String o) {
  int32_t oLen = StringLength(o);
  if (oLen == 0) {
    return s;
  }
  s = StringMaybeGrow(s, oLen);

  for (int32_t i = 0; i < oLen; i++) {
    s[(_StringMeta(s)->length)++] = o[i];
  }
  return s;
}

String StringRepeatChar(char c, int32_t times) {
  if (times <= 0) {
    return StringEmpty();
  }

  String m = (String)__AllocString(times * 2);
  m = m + sizeof(StringMeta);
  _StringMeta(m)->length = 0;
  _StringMeta(m)->cap = times * 2;

  for (int32_t i = 0; i < times; i++) {
    m[(_StringMeta(m)->length)++] = c;
  }

  return m;
}

String StringFromInt(int32_t i) {
  int32_t len = snprintf(0, 0, "%d", i);

  String m = (String)__AllocString(len * 2);
  m = m + sizeof(StringMeta);
  _StringMeta(m)->length = len;
  _StringMeta(m)->cap = len * 2;

  snprintf(m, len + 1, "%d", i);

  return m;
}

String StringFromFloat(float i) {
  int32_t len = snprintf(0, 0, "%f", i);

  String m = (String)__AllocString(len * 2);
  m = m + sizeof(StringMeta);
  _StringMeta(m)->length = len;
  _StringMeta(m)->cap = len * 2;

  snprintf(m, len + 1, "%f", i);

  return m;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !STRING_IMPLEMENTATION_GUARD
#endif // STRING_IMPLEMENTATION
