#ifndef text_h
#define text_h


#include <stddef.h>

#include "textCMap.h"
#include "utilTypes.h"


#define TEXT_BUFFER_USE_FLAG_BYTE

#define TEXT_BUFFER_FLAG_DEFAULT   0x00
#define TEXT_BUFFER_FLAG_FULL      0x01
#define TEXT_BUFFER_FLAG_FULL_MASK (~TEXT_BUFFER_FLAG_FULL)

#ifdef TEXT_BUFFER_USE_FLAG_BYTE
	#define textBufferAddress(buffer) ((buffer)->start)
	#define textBufferIsFull(buffer)  ((buffer)->full)
#else
	#define textBufferAddress(buffer) ((byte_t *)((uintptr_t)(buffer)->start & TEXT_BUFFER_FLAG_FULL_MASK))
	#define textBufferIsFull(buffer)  ((uintptr_t)(buffer)->start & TEXT_BUFFER_FLAG_FULL)
#endif

#define TEXT_UTF8_INVALID_CODE valueInvalid(uint32_t)


/*
** Text buffers represent arrays of characters that can be written
** to dynamically. When the number of characters exceeds the size
** of the buffer, older characters are automatically overwritten.
*/
typedef struct textBuffer {
	// Position to start writing text to.
	byte_t *cursor;
	// Beginning byte for the buffer. When "TEXT_BUFFER_USE_FLAG_BYTE"
	// is defined, the least significant bit of this pointer will be
	// used to store whether or not the buffer has been filled yet.
	byte_t *start;
	// Byte after the last byte of the buffer.
	byte_t *end;
	#ifdef TEXT_BUFFER_USE_FLAG_BYTE
	flags_t full;
	#endif
} textBuffer;


void textBufferInit(textBuffer *const restrict text, const size_t size);

return_t textBufferIsEmpty(const textBuffer *const restrict buffer);
const byte_t *textBufferGetStart(const textBuffer *const restrict buffer);
return_t textBufferFinishedReading(const textBuffer *const restrict buffer, const byte_t *const restrict cursor);

void textBufferWrite(textBuffer *const restrict buffer, const char *restrict str, size_t strLength);
uint32_t textBufferRead(const textBuffer *const restrict text, const byte_t **const restrict cursor);

void textBufferDelete(textBuffer *const restrict text);


#endif