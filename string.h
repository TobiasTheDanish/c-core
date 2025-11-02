#ifndef CORE_STRING_H
#define CORE_STRING_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>

#define StringPush StringAdd

typedef char *String;

String StringEmpty();
String StringFromCString(char *s);
String StringFromCStringL(char *s, int32_t len);

void StringFree(String s);

int32_t StringLength(String s);
int32_t StringCap(String s);

String StringAdd(String s, char c);
String StringAddAll(String s, char *c);
String StringConcat(String s, String o);

#endif // !CORE_STRING_H
#ifdef __cplusplus
}
#endif // __cplusplus

#define STRING_IMPLEMENTATION
#ifdef STRING_IMPLEMENTATION
#ifndef IMPLEMENTATION_GUARD
#define IMPLEMENTATION_GUARD

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef ALLOCATOR_IMPLEMENTATION
#define ALLOCATOR_IMPLEMENTATION
#endif
#include "allocator.h"

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

void StringFree(String s) { __Free(s); }

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
    int32_t minCap = StringLength(s) + addLen;
    if (minCap < StringCap(s) * 2) {
      minCap = StringCap(s) * 2;
    }
    String m = (String)__ReallocString(s, minCap);

    s = m + sizeof(StringMeta);
    _StringMeta(s)->cap = minCap;
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

  s = StringMaybeGrow(s, cLen);

  for (int32_t i = 0; i < cLen; i++) {
    s[(_StringMeta(s)->length)++] = c[i];
  }
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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !IMPLEMENTATION_GUARD
#endif // STRING_IMPLEMENTATION
