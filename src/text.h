#ifndef text_h
#define text_h


#include <stddef.h>

#include "textCMap.h"
#include "utilTypes.h"


#define TEXT_UTF8_INVALID_CODE valueInvalid(uint32_t)


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
	// This actually points to the byte after the last.
	byte_t *end;
} textBuffer;


void textBufferInit(textBuffer *const restrict text, const size_t size);

void textBufferWrite(textBuffer *const restrict text, char *str, size_t strSize);
uint32_t textBufferRead(const textBuffer *const restrict text, const byte_t **const restrict cursor);

void textBufferDelete(textBuffer *const restrict text);


#endif