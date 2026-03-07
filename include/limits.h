// Basic limits.h. I don't know as of writing this what I'll need, so this will be the basic stuff.

#ifndef LIMITS_H
#define LIMITS_H
#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-INT_MAX - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MAX 9223372036854775807L
#define LONG_MIN (-LONG_MAX - 1L)
#define ULONG_MAX 18446744073709551615UL
#define LLONG_MAX 9223372036854775807LL
#define LLONG_MIN    (-LLONG_MAX - 1LL)
#define ULLONG_MAX 18446744073709551615ULL
#endif
