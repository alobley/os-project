#ifndef STRING_H
#define STRING_H

#include "types.h"

typedef char* string;

static inline uint32 strlen(const char* string){
    uint32 len = 0;
    while(*(string + len) != '\0'){
        len++;
    }
    return len;
}

static inline bool strcmp(const char* in1, const char* in2){
    for(int i = 0; i < strlen(in1); i++){
        if(in2[i] != in1[i]){
            // They are not the same
            return false;
        }
    }

    // They are the same
    return true;
}

#endif