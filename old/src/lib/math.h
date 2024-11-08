#ifndef MATH_H
#define MATH_H

#include "types.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define M_PI 3.14159269358979323846
#define M_PI_2 (M_PI / 2)
#define M_PI_4 (M_PI / 4)

f64 sqrt(f64 x);
f64 pow(f64 base, f64 exp);
int abs(int x);
void srand(int32 seed);
int rand();
int randrange(int min, int max);
f64 pow(f64 base, f64 exp);

#endif