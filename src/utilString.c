#include "utilString.h"


#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//Read a line from a file, removing any unwanted stuff!
char *readLineFile(FILE *file, char *line, size_t *lineLength){
	line = fgets(line, 1024, file);
	if(line != NULL){
		*lineLength = strlen(line);

		//Remove comments.
		char *tempPos = strstr(line, "//");
		if(tempPos != NULL){
			*lineLength -= *lineLength - (tempPos - line);
		}

		//"Remove" whitespace characters from the beginning of the line!
		tempPos = &line[*lineLength];
		while(line < tempPos && isspace(*line)){
			++line;
		}
		*lineLength = tempPos - line;

		//"Remove" whitespace characters from the end of the line!
		while(*lineLength > 0 && isspace(line[*lineLength - 1])){
			--*lineLength;
		}

		line[*lineLength] = '\0';
	}


	return(line);
}

//Convert an integer to a string.
//We assume that "str" points to an array with at least (ULONG_MAX_CHARS + 1) characters.
size_t ultostr(unsigned long num, char *str){
	//Special case for when num is equal to 0!
	if(num == 0){
		*str++ = '0';
		*str = '\0';

		return(1);
	}


	size_t length = 0;
	char *curPos = str;

	//Check for a minus sign.
	if(num < 0){
		*curPos = '-';
		num = -num;
	}else{
		*curPos = '\0';
	}

	//Add the digits backwards, starting at the end of the array.
	curPos += 10;
	while(num > 0){
		*curPos-- = '0' + num % 10;
		num /= 10;
		++length;
	}

	//Now copy them over to the front!
	if(*str != '-'){
		memcpy(str, curPos + 1, length);
	}else{
		if(length < 10){
			memcpy(str + 1, curPos + 1, length);
		}
		++length;
	}

	//Add a null terminator and we're set!
	*(str + length) = '\0';


	return(length);
}

void getDelimitedString(char *line, const size_t lineLength, const char *delims, char **strStart, size_t *strLength){
	char *tempStart = NULL;
	//Find the beginning of the string!
	while(tempStart == NULL && *delims != '\0'){
		tempStart = strchr(line, *delims);
		++delims;
	}
	--delims;

	const char *tempEnd = NULL;
	//If we could find a starting delimiter, try and find a closing one!
	if(tempStart != NULL){
		++tempStart;
		tempEnd = strchr(tempStart, *delims);

		//If we can't find a closing delimiter, just use everything up until the first one.
		if(tempEnd == NULL){
			tempEnd = tempStart - 1;
			tempStart = line;
		}

		//Get the string between our delimiters!
		*strStart = tempStart;
		*strLength = tempEnd - tempStart;

	//If we couldn't find any delimiters, use the whole string!
	}else{
		*strStart = line;
		*strLength = lineLength;
	}
}