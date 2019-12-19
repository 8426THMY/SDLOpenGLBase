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
		char *tempPos = strstr(line, "// ");
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