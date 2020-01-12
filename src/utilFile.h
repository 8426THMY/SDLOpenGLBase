#ifndef utilFile_h
#define utilFile_h


#include <stdint.h>
#include <stdio.h>

#include "utilTypes.h"


#define FILE_MAX_LINE_LENGTH 1024


void fileGenerateFullPath(
	const char *filePath, const size_t filePathLength,
	const char *prefix, const size_t prefixLength,
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