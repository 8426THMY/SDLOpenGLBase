#include "utilString.h"


#include <ctype.h>


/*
** Convert an integer to a string. We assume that "str" points
** to an array with at least (ULONG_MAX_CHARS + 1) characters.
**/
size_t ultostr(unsigned long num, char *str){
	size_t length;
	char *curPos;

	// Special case for when num is equal to 0!
	if(num == 0){
		*str = '0';
		++str;
		*str = '\0';

		return(1);
	}

	// Check for a minus sign.
	if(num < 0){
		*str = '-';
		num = -num;
	}else{
		*str = '\0';
	}

	length = 0;
	curPos = str + 10;
	// Add the digits backwards, starting at the end of the array.
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

	// Add a NULL terminator and we're set!
	*(str + length) = '\0';


	return(length);
}

/*
** Find a substring between a pair of delimiters. The parameter
** "delim" represents a single delimiting character. The beginning
** of the substring is written to "token" and its length is returned.
*/
size_t stringDelimited(char *const str, const size_t strLength, const char delim, const char **const restrict token){
	// Find the beginning of the string!
	char *tokStart = strchr(str, delim);

	// If we were able to find a starting
	// delimiter, try and find a closing one!
	if(tokStart != NULL){
		const char *const tokEnd = strchr(++tokStart, delim);
		// Get the string between our delimiters!
		if(tokEnd != NULL){
			*token = tokStart;
			return(tokEnd - tokStart);
		}

		// If we can't find a closing delimiter, just
		// use everything up until the first one.
		*token = str;
		return((tokStart - 1) - str);
	}

	// If we couldn't find a starting
	// delimiter, use the entire string.
	*token = str;
	return(strLength);
}

/*
** Find a substring between a pair of delimiters. The parameter
** "delims" represents an array of possible delimiters. The beginning
** of the substring is written to "token" and its length is returned.
*/
size_t stringMultiDelimited(char *const str, const size_t strLength, const char *delims, const char **const restrict token){
	char *tokStart;
	// Find the beginning of the token!
	while(*delims != '\0' && !(tokStart = strchr(str, *delims))){
		++delims;
	}

	// If we were able to find a starting
	// delimiter, try and find a closing one!
	if(tokStart != NULL){
		const char *const tokEnd = strchr(++tokStart, *delims);
		// Get the string between our delimiters!
		if(tokEnd != NULL){
			*token = tokStart;
			return(tokEnd - tokStart);
		}

		// If we can't find a closing delimiter, just
		// use everything up until the first one.
		*token = str;
		return((tokStart - 1) - str);
	}

	// If we couldn't find a starting
	// delimiter, use the entire string.
	*token = str;
	return(strLength);
}

/*
** Return a pointer to the next occurrence of a specified delimiter,
** where "delims" is an array of potential delimiters to look for.
*/
char *stringTokenDelims(const char *const restrict str, const char *delims){
	char *tokEnd;
	// Find the end of the token!
	while(*delims != '\0' && !(tokEnd = strchr(str, *delims))){
		++delims;
	}

	return(tokEnd);
}