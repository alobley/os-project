#ifndef MATH_H
#define MATH_H

#include "types.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define M_PI 3.14159269358979323846
#define M_PI_2 (M_PI / 2)
#define M_PI_4 (M_PI / 4)

float64 sqrt(float64 x);
float64 pow(float64 base, float64 exp);
int abs(int x);
void srand(int32 seed);
int rand();
int randrange(int min, int max);
float64 pow(float64 base, float64 exp);

#endif