#include "string.h"

int strcmp(const char *s1, const char *s2) {
    // Iterate over each character of both strings
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    // Return the difference between the characters where they first differ
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void strcpy(char* s1, const char* s2, size_t length){
    for(size_t i = 0; i <= length; i++){
        s1[i] = s2[i];
    }
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}