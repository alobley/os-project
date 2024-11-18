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

// Apparently this is the opposite of the standard library implementation, which is honestly a crime.
// It should return true if two strings are equal. I will die on this hill.
static inline bool strcmp(const char* in1, const char* in2){
    int i;
    int len1 = strlen(in1);
    int len2 = strlen(in2);

    if(len1 != len2){
        // If they aren't the same length, they aren't the same.
        return false;
    }

    for(i = 0; i < len1; i++){
        if(in2[i] != in1[i]){
            // They are not the same
            return false;
        }
    }

    // They are the same
    return true;
}


static inline bool strncmp(const char* in1, const char* in2, int term){
    for(int i = 0; i < term; i++){
        if(in1[i] == '\0' && in2[i] == '\0'){
            return true;
        }
        
        if(in2[i] != in1[i] || in1[i] == '\0' || in2[i] == '\0'){
            // Check failed
            return false;
        }
    }

    // Up to the given point, they are the same
    return true;
}

#endif