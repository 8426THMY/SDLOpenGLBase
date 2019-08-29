#ifndef utilString_h
#define utilString_h


#include <stdio.h>
#include <stdint.h>
#include <string.h>


// These indicate the maximum number of characters
// required to store a number of each type.
#define ULONG_MAX_CHARS 10


#define getToken(str, delim) (strchr(str, delim))


char *readLineFile(FILE *file, char *line, size_t *lineLength);
size_t ultostr(unsigned long num, char *str);
char *getDelimitedString(char *str, const size_t strLength, const char delim, size_t *outLength);
char *getMultiDelimitedString(char *str, const size_t strLength, const char *delims, size_t *outLength);
char *getTokenDelims(const char *str, const char *delims);


#endif