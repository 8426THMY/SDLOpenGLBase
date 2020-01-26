#include "utilFile.h"


#include <ctype.h>
#include <string.h>


// Prepend a resource's prefix to its path!
void fileGenerateFullPath(
	const char *filePath, const size_t filePathLength,
	const char *prefix, const size_t prefixLength,
	char *fullPath
){

	memcpy(fullPath, prefix, prefixLength);
	fullPath += prefixLength;

	memcpy(fullPath, filePath, filePathLength);
	fullPath += filePathLength;

	*fullPath = '\0';
}

// Read a line from a file, removing any unwanted stuff!
char *fileReadLine(FILE *file, char *line, size_t *lineLength){
	line = fgets(line, FILE_MAX_LINE_LENGTH, file);
	if(line != NULL){
		*lineLength = strlen(line);

		// Remove comments.
		char *tempPos = strstr(line, "//");
		if(tempPos != NULL){
			*lineLength -= *lineLength - (tempPos - line);
		}

		// "Remove" whitespace characters from the beginning of the line!
		tempPos = &line[*lineLength];
		while(line < tempPos && isspace(*line)){
			++line;
		}
		*lineLength = tempPos - line;

		// "Remove" whitespace characters from the end of the line!
		while(*lineLength > 0 && isspace(line[*lineLength - 1])){
			--*lineLength;
		}

		line[*lineLength] = '\0';
	}


	return(line);
}


// Convert a 16-bit little-endian value to the host's format.
uint16_t littleEndianToHost16(const uint16_t val){
	const byte_t *bytes = (byte_t *)&val;
	return(
		((uint16_t)bytes[0] << 8) |
		(uint16_t)bytes[1]
	);
}

// Convert a 16-bit big-endian value to the host's format.
uint16_t bigEndianToHost16(const uint16_t val){
	const byte_t *bytes = (byte_t *)&val;
	return(
		(uint16_t)bytes[0] |
		((uint16_t)bytes[1] << 8)
	);
}

// Convert a 32-bit little-endian value to the host's format.
uint32_t littleEndianToHost32(const uint32_t val){
	const byte_t *bytes = (byte_t *)&val;
	return(
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3]
	);
}

// Convert a 32-bit big-endian value to the host's format.
uint32_t bigEndianToHost32(const uint32_t val){
	const byte_t *bytes = (byte_t *)&val;
	return(
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24)
	);
}


byte_t readByte(FILE *file){
	byte_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint16_t readUint16(FILE *file){
	uint16_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint16_t readUint16LE(FILE *file){
	byte_t bytes[2];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint16_t)bytes[0] << 8) |
		(uint16_t)bytes[1]
	);
}

uint16_t readUint16BE(FILE *file){
	byte_t bytes[2];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		(uint16_t)bytes[0] |
		((uint16_t)bytes[1] << 8)
	);
}

uint32_t readUint32(FILE *file){
	uint32_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint32_t readUint32LE(FILE *file){
	byte_t bytes[4];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3]
	);
}

uint32_t readUint32BE(FILE *file){
	byte_t bytes[4];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24)
	);
}

float readFloat(FILE *file){
	uint32_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

float readFloatLE(FILE *file){
	byte_t bytes[4];
	float val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	*((uint32_t *)&val) =
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3];
	return(val);
}

float readFloatBE(FILE *file){
	byte_t bytes[4];
	float val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	*((uint32_t *)&val) =
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24);
	return(val);
}

double readDouble(FILE *file){
	double val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

double readDoubleLE(FILE *file){
	byte_t bytes[4];
	double val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	*((uint64_t *)&val) =
		((uint64_t)bytes[0] << 56) |
		((uint64_t)bytes[1] << 48) |
		((uint64_t)bytes[2] << 40) |
		((uint64_t)bytes[3] << 32) |
		((uint64_t)bytes[4] << 24) |
		((uint64_t)bytes[5] << 16) |
		((uint64_t)bytes[6] << 8)  |
		(uint64_t)bytes[7];
	return(val);
}

double readDoubleBE(FILE *file){
	byte_t bytes[8];
	double val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	*((uint64_t *)&val) =
		(uint64_t)bytes[0]         |
		((uint64_t)bytes[1] << 8)  |
		((uint64_t)bytes[2] << 16) |
		((uint64_t)bytes[3] << 24) |
		((uint64_t)bytes[4] << 32) |
		((uint64_t)bytes[5] << 40) |
		((uint64_t)bytes[6] << 48) |
		((uint64_t)bytes[7] << 56);
	return(val);
}