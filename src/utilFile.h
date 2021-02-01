#ifndef utilFile_h
#define utilFile_h


#include <stdint.h>
#include <stdio.h>

#include "utilTypes.h"


// Windows supports Linux path delimiters, so
// we could really just always use Linux paths.
#ifdef _WIN32
	#define FILE_PATH_DELIMITER        '\\'
	#define FILE_PATH_DELIMITER_STR    "\\"
	#define FILE_PATH_DELIMITER_UNUSED '/'
#else
	#define FILE_PATH_DELIMITER        '/'
	#define FILE_PATH_DELIMITER_STR    "/"
	#define FILE_PATH_DELIMITER_UNUSED '\\'
#endif

#define FILE_MAX_LINE_LENGTH 1024
#ifdef _WIN32
	#include <windows.h>
	#define FILE_MAX_PATH_LENGTH MAX_PATH
#else
	#include <limits.h>
	#define FILE_MAX_PATH_LENGTH PATH_MAX
#endif


#warning "We use 'FILE_MAX_PATH_LENGTH' too much when we know the length. It would be better to allocate on the heap."


return_t fileSetWorkingDirectory(char *const restrict dir, size_t *const restrict pathLength);
size_t fileParseResourcePath(char *restrict resPath, char *line, const size_t lineLength, char **const endPtr);
void fileGenerateFullResourcePath(
	const char *const restrict prefix, const size_t prefixLength,
	const char *const restrict filePath, const size_t filePathLength,
	char *fullPath
);
char *fileReadLine(FILE *const restrict file, char *line, size_t *const restrict lineLength);


uint16_t littleEndianToHost16(const uint16_t val);
uint16_t bigEndianToHost16(const uint16_t val);
uint32_t littleEndianToHost32(const uint32_t val);
uint32_t bigEndianToHost32(const uint32_t val);

byte_t readByte(FILE *const restrict file);
uint16_t readUint16(FILE *const restrict file);
uint16_t readUint16LE(FILE *const restrict file);
uint16_t readUint16BE(FILE *const restrict file);
uint32_t readUint32(FILE *const restrict file);
uint32_t readUint32LE(FILE *const restrict file);
uint32_t readUint32BE(FILE *const restrict file);
float readFloat(FILE *const restrict file);
float readFloatLE(FILE *const restrict file);
float readFloatBE(FILE *const restrict file);
double readDouble(FILE *const restrict file);
double readDoubleLE(FILE *const restrict file);
double readDoubleBE(FILE *const restrict file);


#endif