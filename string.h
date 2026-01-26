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
String StringCopy(String s);
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

Bool StringToFloat(String s, float *res);

/** Returns index of the specified char, starting from index of pos. Returns -1
 * if not found */
int32_t StringFindChar(String s, char c, int32_t pos);
String StringSubStr(String s, int32_t offset, int32_t end);
Bool StringEqualsCStr(String s, char *cstr);
Bool StringIsSpace(String s, int32_t idx);
Bool StringIsAlpha(String s, int32_t idx);
Bool StringIsNumeric(String s, int32_t idx);
/** Returns true if the char of s at idx is equal to any of the chars in chars
 */
Bool StringIsAny(String s, int32_t idx, String chars);

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

String StringCopy(String s) {
  if (!s || StringLength(s) == 0) {
    return StringEmpty();
  }

  return StringFromCStringL(s, StringLength(s));
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

Bool StringToFloat(String s, float *res) {
  int32_t len = StringLength(s);

  Bool isNegative = false;
  Bool inDecimals = false;

  uint32_t base = 0;
  uint32_t decimal = 0;
  uint32_t decimalExponent = 1;

  *res = 0;
  for (int32_t i = 0; i < len; i++) {
    char c = s[i];
    if ('0' <= c && c <= '9') {
      if (inDecimals) {
        decimal = decimal * 10 + (c - '0');
        decimalExponent *= 10;
      } else {
        base = base * 10 + (c - '0');
      }
    } else if (c == '.') {
      inDecimals = true;
    } else if (i = 0 && c == '-') {
      isNegative = true;
    } else {
      *res = 0;
      return false;
    }
  }

  float f = (float)base;
  if (inDecimals) {
    f += ((float)decimal) / decimalExponent;
  }
  if (isNegative) {
    f = -f;
  }
  *res = f;

  return true;
}

int32_t StringFindChar(String s, char c, int32_t pos) {
  if (pos >= _StringMeta(s)->length) {
    return -1;
  }

  for (int32_t i = pos; i < _StringMeta(s)->length; i++) {
    if (s[i] == c) {
      return i;
    }
  }

  return -1;
}

String StringSubStr(String s, int32_t offset, int32_t len) {
  if (offset >= 0 && len > 0 && len > offset && len < StringLength(s)) {
    return StringFromCStringL(s + offset, len - offset);
  } else if (len <= 0 || offset - len == 0) {
    return StringEmpty();
  } else {
    return StringFromCStringL(s + offset, StringLength(s) - offset);
  }
}

Bool StringEqualsCStr(String s, char *cstr) {
  int32_t cLen = CStringLength(cstr);
  int32_t sLen = StringLength(s);
  if (cLen != sLen) {
    return false;
  }

  for (int32_t i = 0; i < sLen; i++) {
    if (s[i] != cstr[i]) {
      return false;
    }
  }

  return true;
}

Bool StringIsSpace(String s, int32_t idx) {
  char c = s[idx];
  switch (c) {
  case ' ':
  case '\n':
  case '\r':
  case '\t':
  case '\v':
  case '\f':
    return true;

  default:
    return false;
  }
}

Bool StringIsAlpha(String s, int32_t idx) {
  char c = s[idx];
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

Bool StringIsNumeric(String s, int32_t idx) {
  char c = s[idx];
  return '0' <= c && c <= '9';
}

Bool StringIsAny(String s, int32_t idx, String chars) {
  int32_t charsLen = StringLength(chars);

  char c = s[idx];
  for (int32_t i = 0; i < charsLen; i++) {
    if (c == chars[i]) {
      return true;
    }
  }
  return false;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !STRING_IMPLEMENTATION_GUARD
#endif // STRING_IMPLEMENTATION
