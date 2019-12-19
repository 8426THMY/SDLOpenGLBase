#ifndef utilFile_h
#define utilFile_h


#include <stdio.h>


#define FILE_MAX_LINE_LENGTH 1024


void fileGenerateFullPath(
	const char *filePath, const size_t filePathLength,
	const char *prefix, const size_t prefixLength,
	char *fullPath
);
char *fileReadLine(FILE *file, char *line, size_t *lineLength);


#endif