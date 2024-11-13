#ifndef STRING_H
#define STRING_H

#include "types.h"

int strcmp(const char *s1, const char *s2);
void strcpy(char* s1, const char* s2, size_t length);
void* memcpy(void* dest, const void* src, size_t n);

#endif