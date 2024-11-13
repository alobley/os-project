#include "math.h"
#include "time.h"

float64 sqrt(float64 x){
    if(x < 0) return -1;        // Can't sqrt negatives
    float64 result;
    asm volatile("fsqrt" : "=t" (result) : "0" (x));
    return result;
}

int abs(int x){
    return (x < 0) ? -x : x;
}

uint32 randSeed = 0;

// Seed the random number generator. If not called, the seed will be 0. While that's fine, it won't really be random.
void srand(int32 seed){
    if(seed == 0){
        // If we seed with 0, make the seed the current system time
        randSeed = (int32)GetTicks();
    }else{
        randSeed = seed;
    }
}

int rand(){
    randSeed = randSeed * 1103515245 + 12345;
    return (int32)(randSeed / 65536) % 32768;
}

int randrange(int min, int max){
    randSeed = randSeed * 1103515245 + 12345;
    return min + (int32)(randSeed / 65536) % (max - min + 1);
}

float64 pow(float64 base, float64 exp){
    if(exp == 0) return 1;
    float64 result = 1;
    for(int i = 0; i < exp; ++i){
        result *= base;
    }
    return result;
}

double factorial(int n){
    if(n == 0 || n == 1) return 1.0;
    double result = 1.0;
    for(int i = 2; i <= n; i++) result *= i;
    return result;
}

double sin(double x, int precision){
    double result = 0.0;
    for(int i = 0; i < precision; i++){
        double term = pow(-1, i) * pow(x, 2 * i + 1) / factorial(2 * i + 1);
        result += term;
    }
    return result;
}

double cos(double x, int precision){
    double result = 0.0;
    for(int i = 0; i < precision; i++){
        double term = pow(-1, i) * pow(x, 2 * i) / factorial(2 * i);
        result += term;
    }
    return result;
}

double tan(double x, int precision){
    double c = cos(x, precision);
    if(c == 0){
        // Return the maximum possible floating-point number (actual answer is infinity)
        return 1.8e38;
    }
    return sin(x, precision) / c;
}