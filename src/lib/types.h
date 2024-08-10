#ifndef TYPES_H
#define TYPES_H

// Null pointer
#define NULL ((void *)0)

// Unsigned types
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Signed types
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

// size_t, for when you don't want to use uint64.
typedef uint64 size_t;

// Floats
typedef float f32;
typedef double f64;

typedef uint64 uintptr_t;

// Booleans
typedef uint8 bool;
#define true 1
#define false 0

// Limits
#define UINT_MAX 0xFFFFFFFF
#define ULONG_MAX 0xFFFFFFFFFFFFFFFF

#define INT_MAX 2147483647
#define LONG_MAX 9223372036854775807

#define INT_MIN (-INT_MAX - 1)
#define LONG_MIN (-LONG_MAX - 1)

#endif