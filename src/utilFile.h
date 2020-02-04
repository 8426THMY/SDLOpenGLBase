#ifndef utilFile_h
#define utilFile_h


#include <stdint.h>
#include <stdio.h>

#include "utilTypes.h"


// Windows supports Linux path delimiters, so
// we could really just always use Linux paths.
#ifdef _WIN32
	#define FILE_PATH_DELIMITER        '\\'
	#define FILE_PATH_DELIMITER_UNUSED '/'
#else
	#define FILE_PATH_DELIMITER        '/'
	#define FILE_PATH_DELIMITER_UNUSED '\\'
#endif

#define FILE_MAX_LINE_LENGTH 1024
#ifdef _WIN32
	#define FILE_MAX_PATH_LENGTH MAX_PATH
#else
	#include <limits.h>
	#define FILE_MAX_PATH_LENGTH PATH_MAX
#endif


size_t fileParseResourcePath(char *resPath, char *line, const size_t lineLength, char **endPtr);
void fileGenerateFullResourcePath(
	const char *prefix, const size_t prefixLength,
	const char *filePath, const size_t filePathLength,
	char *fullPath
);
char *fileReadLine(FILE *file, char *line, size_t *lineLength);


uint16_t littleEndianToHost16(const uint16_t val);
uint16_t bigEndianToHost16(const uint16_t val);
uint32_t littleEndianToHost32(const uint32_t val);
uint32_t bigEndianToHost32(const uint32_t val);

byte_t readByte(FILE *file);
uint16_t readUint16(FILE *file);
uint16_t readUint16LE(FILE *file);
uint16_t readUint16BE(FILE *file);
uint32_t readUint32(FILE *file);
uint32_t readUint32LE(FILE *file);
uint32_t readUint32BE(FILE *file);
float readFloat(FILE *file);
float readFloatLE(FILE *file);
float readFloatBE(FILE *file);
double readDouble(FILE *file);
double readDoubleLE(FILE *file);
double readDoubleBE(FILE *file);


#endif