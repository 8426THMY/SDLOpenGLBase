#ifndef strUtil_h
#define strUtil_h


//These indicate the maximum number of characters
//required to store a number of each type.
#define ULONG_MAX_CHARS 10


#include <stdio.h>
#include <stdint.h>


char *readLineFile(FILE *file, char *line, size_t *lineLength);
size_t ultostr(unsigned long num, char *str);
void getDelimitedString(char *line, const size_t lineLength, const char *delims, char **strStart, size_t *strLength);


#endif