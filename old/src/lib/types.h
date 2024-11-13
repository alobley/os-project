#ifndef TYPES_H
#define TYPES_H

#define PACKED __attribute__((packed))
#define ALIGNED(num) __attribute__aligned((num))

// NULL pointer
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

// Size_t, which is for when I want to feel special and quirky and decide not to use uint64.
typedef uint32 size_t;

// Floats
typedef float f32;
typedef double f64;

typedef uint32 uintptr_t;

// Booleans
#define bool unsigned char
#define true 1
#define false 0

// Max values
#define UINT_MAX -1U
#define ULONG_MAX -1ULL

#endif
