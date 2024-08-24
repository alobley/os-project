#include "math.h"
#include "time.h"

#define M_PI 3.14159269358979323846

f64 sqrt(f64 x){
    if(x < 0) return -1;        // Can't sqrt negatives
    f64 result;
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
    while((int32)(randSeed / 65536) % 32768 > max || (int32)(randSeed / 65536) % 32768 < min){
        randSeed = randSeed * 1103515245 + 12345;
    }
    return (int32)(randSeed / 65536) % 32768;
}

f64 pow(f64 base, f64 exp){
    if(exp == 0) return 1;
    f64 result = 1;
    for(int i = 0; i < exp; ++i){
        result *= base;
    }
    return result;
}