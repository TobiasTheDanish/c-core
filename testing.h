#ifndef TESTING_H
#define TESTING_H
#include "common.h"

#ifndef TEST_IMPLEMENTATION

#define assertNull(p)
#define assertNotNull(p)
#define assertTrue(actual)
#define assertFalse(actual)

#define assertEqualsInt(actual, expected)
#define assertEqualsUint(actual, expected)
#define assertEqualsFloat(actual, expected)
#define assertEqualsDouble(actual, expected)
#define assertEqualsPtr(actual, expected)
#define assertEqualsChar(actual, expected)

#define assertNotEqualsInt(actual, expected)
#define assertNotEqualsUint(actual, expected)
#define assertNotEqualsFloat(actual, expected)
#define assertNotEqualsDouble(actual, expected)
#define assertNotEqualsPtr(actual, expected)
#define assertNotEqualsChar(actual, expected)

#endif // !TEST_IMPLEMENTATION

#ifdef TEST_IMPLEMENTATION

#include "testing/testing.c"

#endif // TEST_IMPLEMENTATION

#endif // !TESTING_H
