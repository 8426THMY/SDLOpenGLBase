#include "utilFile.h"


#include <ctype.h>
#include <string.h>

#include "utilMath.h"


#ifdef _WIN32
	#include <direct.h>
	#define chdir(dir) _chdir(dir)
#else
	#include <unistd.h>
#endif


/*
** Change the program's working directory and return whether or not it succeeded.
** Note that this will cause undefined behaviour if "dir" is a string literal.
*/
return_t fileSetWorkingDirectory(char *const restrict dir, size_t *const restrict pathLength){
	char *const lastDelim = strrchr(dir, FILE_PATH_DELIMITER) + 1;
	const char lastChar = *lastDelim;
	// Add a NUL terminator to cut off the executable name
	// and try to change the current working directory.
	const return_t success = (*lastDelim = '\0', chdir(dir));

	if(pathLength != NULL){
		*pathLength = lastDelim - dir;
	}

	// Add the file path delimiter back.
	*lastDelim = lastChar;

	return(success);
}

/*
** Copy a resource's path from "line" into "resPath", converting any folder
** delimiters. If the path is between quotations marks, they will be removed too.
*/
size_t fileParseResourcePath(char *resPath, char *const restrict line, const size_t lineLength, char **const restrict endPtr){
	const char *pathStart = strchr(line, '"');
	char *pathEnd = NULL;
	const char *delimiter;

	// Find the beginning and end of the resource path string.
	if(pathStart != NULL){
		++pathStart;
		pathEnd = strrchr(line, '"');
	}
	if(pathStart == pathEnd){
		pathStart = line;
		pathEnd   = &line[lineLength];
	}

	// Copy the path and convert any invalid folder delimiters.
	for(delimiter = pathStart; delimiter != pathEnd; ++resPath, ++delimiter){
		if(*delimiter == FILE_PATH_DELIMITER_UNUSED){
			*resPath = FILE_PATH_DELIMITER;
		}else{
			*resPath = *delimiter;
		}
	}

	*resPath = '\0';
	if(endPtr != NULL){
		*endPtr = pathEnd;
	}


	return((size_t)(pathEnd - pathStart));
}

// Prepend a resource's prefix to its path!
void fileGenerateFullResourcePath(
	const char *const restrict prefix, const size_t prefixLength,
	const char *const restrict filePath, const size_t filePathLength,
	char *fullPath
){

	memcpy(fullPath, prefix, prefixLength);
	fullPath += prefixLength;

	memcpy(fullPath, filePath, filePathLength);
	fullPath += filePathLength;

	*fullPath = '\0';
}

// Read a line from a file, removing any unwanted stuff!
char *fileReadLine(FILE *const restrict file, char *line, size_t *const restrict lineLength){
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
	const byte_t *const bytes = (byte_t *)&val;
	return(
		((uint16_t)bytes[0] << 8) |
		(uint16_t)bytes[1]
	);
}

// Convert a 16-bit big-endian value to the host's format.
uint16_t bigEndianToHost16(const uint16_t val){
	const byte_t *const bytes = (byte_t *)&val;
	return(
		(uint16_t)bytes[0] |
		((uint16_t)bytes[1] << 8)
	);
}

// Convert a 32-bit little-endian value to the host's format.
uint32_t littleEndianToHost32(const uint32_t val){
	const byte_t *const bytes = (byte_t *)&val;
	return(
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3]
	);
}

// Convert a 32-bit big-endian value to the host's format.
uint32_t bigEndianToHost32(const uint32_t val){
	const byte_t *const bytes = (byte_t *)&val;
	return(
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24)
	);
}


byte_t readByte(FILE *const restrict file){
	byte_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint16_t readUint16(FILE *const restrict file){
	uint16_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint16_t readUint16LE(FILE *const restrict file){
	byte_t bytes[2];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint16_t)bytes[0] << 8) |
		(uint16_t)bytes[1]
	);
}

uint16_t readUint16BE(FILE *const restrict file){
	byte_t bytes[2];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		(uint16_t)bytes[0] |
		((uint16_t)bytes[1] << 8)
	);
}

uint32_t readUint32(FILE *const restrict file){
	uint32_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

uint32_t readUint32LE(FILE *const restrict file){
	byte_t bytes[4];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3]
	);
}

uint32_t readUint32BE(FILE *const restrict file){
	byte_t bytes[4];
	fread((void *)bytes, sizeof(bytes), 1, file);
	return(
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24)
	);
}

float readFloat(FILE *const restrict file){
	uint32_t val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

float readFloatLE(FILE *const restrict file){
	byte_t bytes[4];
	bitFloat val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	val.l =
		((uint32_t)bytes[0] << 24) |
		((uint32_t)bytes[1] << 16) |
		((uint32_t)bytes[2] << 8)  |
		(uint32_t)bytes[3];
	return(val.f);
}

float readFloatBE(FILE *const restrict file){
	byte_t bytes[4];
	bitFloat val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	val.l =
		(uint32_t)bytes[0]         |
		((uint32_t)bytes[1] << 8)  |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24);
	return(val.f);
}

double readDouble(FILE *const restrict file){
	double val;
	fread((void *)&val, sizeof(val), 1, file);
	return(val);
}

double readDoubleLE(FILE *const restrict file){
	byte_t bytes[8];
	bitDouble val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	val.l =
		((uint64_t)bytes[0] << 56) |
		((uint64_t)bytes[1] << 48) |
		((uint64_t)bytes[2] << 40) |
		((uint64_t)bytes[3] << 32) |
		((uint64_t)bytes[4] << 24) |
		((uint64_t)bytes[5] << 16) |
		((uint64_t)bytes[6] << 8)  |
		(uint64_t)bytes[7];
	return(val.d);
}

double readDoubleBE(FILE *const restrict file){
	byte_t bytes[8];
	bitDouble val;
	fread((void *)bytes, sizeof(bytes), 1, file);
	val.l =
		(uint64_t)bytes[0]         |
		((uint64_t)bytes[1] << 8)  |
		((uint64_t)bytes[2] << 16) |
		((uint64_t)bytes[3] << 24) |
		((uint64_t)bytes[4] << 32) |
		((uint64_t)bytes[5] << 40) |
		((uint64_t)bytes[6] << 48) |
		((uint64_t)bytes[7] << 56);
	return(val.d);
}