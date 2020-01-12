#include "utilString.h"


#include <ctype.h>


/*
** Convert an integer to a string. We assume that "str" points
** to an array with at least (ULONG_MAX_CHARS + 1) characters.
**/
size_t ultostr(unsigned long num, char *str){
	// Special case for when num is equal to 0!
	if(num == 0){
		*str++ = '0';
		*str = '\0';

		return(1);
	}


	size_t length = 0;
	char *curPos = str;

	// Check for a minus sign.
	if(num < 0){
		*curPos = '-';
		num = -num;
	}else{
		*curPos = '\0';
	}

	// Add the digits backwards, starting at the end of the array.
	curPos += 10;
	while(num > 0){
		*curPos-- = '0' + num % 10;
		num /= 10;
		++length;
	}

	// Now copy them over to the front!
	if(*str != '-'){
		memcpy(str, curPos + 1, length);
	}else{
		if(length < 10){
			memcpy(str + 1, curPos + 1, length);
		}
		++length;
	}

	// Add a null terminator and we're set!
	*(str + length) = '\0';


	return(length);
}

/*
** Find a substring between a pair of delimiters. The parameter "delim"
** represents a single delimiting character. The length of the substring
** is written to "outLength" and a pointer to where it begins is returned.
*/
char *getDelimitedString(char *str, const size_t strLength, const char delim, size_t *outLength){
	// Find the beginning of the string!
	char *tokStart = strchr(str, delim);

	// If we were able to find a starting
	// delimiter, try and find a closing one!
	if(tokStart != NULL){
		const char *tokEnd;

		++tokStart;
		tokEnd = strchr(tokStart, delim);

		// If we can't find a closing delimiter, just
		// use everything up until the first one.
		if(tokEnd == NULL){
			*outLength = tokStart - 1 - str;
			return(str);
		}

		// Get the string between our delimiters!
		*outLength = tokEnd - tokStart;
		return(tokStart);
	}

	// If we couldn't find a starting
	// delimiter, use the entire string.
	*outLength = strLength;
	return(str);
}

/*
** Find a substring between a pair of delimiters. The parameter "delims"
** represents an array of possible delimiters. The length of the substring
** is written to "outLength" and a pointer to where it begins is returned.
*/
char *getMultiDelimitedString(char *str, const size_t strLength, const char *delims, size_t *outLength){
	char *tokStart;
	// Find the beginning of the token!
	while(*delims != '\0' && !(tokStart = strchr(str, *delims))){
		++delims;
	}

	// If we were able to find a starting
	// delimiter, try and find a closing one!
	if(tokStart != NULL){
		const char *tokEnd;

		++tokStart;
		tokEnd = strchr(tokStart, *delims);

		// If we can't find a closing delimiter, just
		// use everything up until the first one.
		if(tokEnd == NULL){
			*outLength = tokStart - 1 - str;
			return(str);
		}

		// Get the string between our delimiters!
		*outLength = tokEnd - tokStart;
		return(tokStart);
	}

	// If we couldn't find a starting
	// delimiter, use the entire string.
	*outLength = strLength;
	return(str);
}

/*
** Return a pointer to the next occurrence of a specified delimiter,
** where "delims" is an array of potential delimiters to look for.
*/
char *getTokenDelims(const char *str, const char *delims){
	char *tokEnd;
	// Find the end of the token!
	while(*delims != '\0' && !(tokEnd = strchr(str, *delims))){
		++delims;
	}

	return(tokEnd);
}