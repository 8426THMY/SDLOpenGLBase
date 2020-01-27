#ifndef text_h
#define text_h


#include "font.h"
#include "utilTypes.h"


/*
** Text buffers represent arrays of characters that can be written
** to dynamically. When the number of characters exceeds the size
** of the buffer, older characters are automatically overwritten.
*/
typedef struct textBuffer {
	// Position to start reading text from.
	byte_t *offset;
	// Beginning and end bytes for the buffer.
	byte_t *start;
	byte_t *end;

	font *typeface;
} textBuffer;


#endif