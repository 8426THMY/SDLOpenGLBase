#ifndef utilString_h
#define utilString_h


#include <stdio.h>
#include <stdint.h>
#include <string.h>


// These indicate the maximum number of characters
// required to store a number of each type.
#define ULONG_MAX_CHARS 10


size_t ultostr(unsigned long num, char *str);
char *stringDelimited(char *const restrict str, const size_t strLength, const char delim, size_t *const restrict outLength);
char *stringMultiDelimited(char *const restrict str, const size_t strLength, const char *delims, size_t *const restrict outLength);
char *stringTokenDelims(const char *const restrict str, const char *delims);


#endif