#include "text.h"


#include "memoryManager.h"


#ifndef TEXT_BUFFER_USE_FLAG_BYTE
	#define textBufferSetFull(buffer)   ((byte_t *)((uintptr_t)(buffer)->start | TEXT_BUFFER_FLAG_FULL))
	#define textBufferUnsetFull(buffer) ((byte_t *)((uintptr_t)(buffer)->start & TEXT_BUFFER_FLAG_FULL_MASK))
#endif


// By finding the smallest value from the following list
// that our character's first byte is less than, we can
// determine how many bytes that character should use.
#define UTF8_1_BYTE_LIMIT     0x7F
#define UTF8_2_BYTE_LIMIT     0xDF
#define UTF8_3_BYTE_LIMIT     0xEF
#define UTF8_4_BYTE_LIMIT     0xFF
// Any extra bytes should be less than this value.
#define UTF8_EXTRA_BYTE_LIMIT 0x00BF

// We can use these masks to get the
// codepoints from the encoded bytes.
#define UTF8_1_BYTE_CODEPOINT_MASK     0x7F
#define UTF8_2_BYTE_CODEPOINT_MASK     0x1F
#define UTF8_3_BYTE_CODEPOINT_MASK     0x0F
#define UTF8_4_BYTE_CODEPOINT_MASK     0x07
#define UTF8_EXTRA_BYTE_CODEPOINT_MASK 0x3F

// Bytes should begin with these header bits.
#define UTF8_1_BYTE_HEADER     0x00
#define UTF8_2_BYTE_HEADER     0xC0
#define UTF8_3_BYTE_HEADER     0xE0
#define UTF8_4_BYTE_HEADER     0xF0
#define UTF8_EXTRA_BYTE_HEADER 0x80

// We can use these masks to validate the bytes' headers.
#define UTF8_1_BYTE_HEADER_MASK     0x80
#define UTF8_2_BYTE_HEADER_MASK     0xE0
#define UTF8_3_BYTE_HEADER_MASK     0xF0
#define UTF8_4_BYTE_HEADER_MASK     0xF8
#define UTF8_EXTRA_BYTE_HEADER_MASK 0xC0

#define UTF8_INVALID_BYTE 0xFF

#define byteIsValid(byte) (((byte) & UTF8_EXTRA_BYTE_HEADER_MASK) == UTF8_EXTRA_BYTE_HEADER)


// Forward-declare any helper functions!
static byte_t readBufferByte(const textBuffer *const restrict buffer, const byte_t **const restrict cursor);


void textBufferInit(textBuffer *const restrict buffer, const size_t bufferLength){
	buffer->cursor = memoryManagerGlobalAlloc(bufferLength);
	if(buffer->cursor == NULL){
		/** MALLOC FAILED **/
	}
	buffer->start = buffer->cursor;
	buffer->end = &buffer->start[bufferLength];
	#ifdef TEXT_BUFFER_USE_FLAG_BYTE
	buffer->full = TEXT_BUFFER_FLAG_DEFAULT;
	#endif
}


return_t textBufferIsEmpty(const textBuffer *const restrict buffer){
	#ifdef TEXT_BUFFER_USE_FLAG_BYTE
	return(!textBufferIsFull(buffer) && buffer->cursor == buffer->start);
	#else
	// If the buffer is full, the least significant bit
	// of its pointer will be '1', so this will be false.
	return(buffer->cursor == buffer->start);
	#endif
}

const byte_t *textBufferGetStart(const textBuffer *const restrict buffer){
	// When the buffer is full, the text starts and ends
	// at the cursor. Otherwise, we begin at the buffer.
	return(textBufferIsFull(buffer) ? buffer->cursor : buffer->start);
}

return_t textBufferFinishedReading(const textBuffer *const restrict buffer, const byte_t *const restrict cursor){
	return(cursor == buffer->cursor);
}


// Write a string to a text buffer!
#warning "If we overwrite part of a UTF8 character, we'll probably get weird results."
void textBufferWrite(textBuffer *const restrict buffer, const char *restrict str, size_t strLength){
	#ifdef TEXT_BUFFER_USE_FLAG_BYTE
	const size_t bufferLength = (size_t)memorySubPointer(buffer->end, buffer->start);
	#else
	byte_t *const buffer = textBufferAddress(buffer->start);
	const size_t bufferLength = (size_t)memorySubPointer(buffer->end, buffer);
	#endif

	// If the string is larger than the buffer, add
	// the characters we have room for from the end.
	if(strLength >= bufferLength){
		#ifdef TEXT_BUFFER_USE_FLAG_BYTE
		memcpy(buffer->start, &str[strLength - bufferLength], bufferLength);
		buffer->cursor = buffer->start;
		buffer->full = TEXT_BUFFER_FLAG_FULL;
		#else
		memcpy(buffer, &str[strLength - bufferLength], bufferLength);
		buffer->cursor = buffer;
		buffer->start = textBufferSetFull(buffer->start);
		#endif

	// Otherwise, we can fill the buffer with the entire string.
	}else{
		const size_t remainingBuffer = (size_t)memorySubPointer(buffer->end, buffer->cursor);

		// If the string is too big to fit in the remaining bytes
		// of the buffer, fill it to the end and wrap to the start.
		if(strLength >= remainingBuffer){
			memcpy(buffer->cursor, str, remainingBuffer);
			#ifdef TEXT_BUFFER_USE_FLAG_BYTE
			buffer->cursor = buffer->start;
			buffer->full = TEXT_BUFFER_FLAG_FULL;
			#else
			buffer->cursor = buffer;
			buffer->start = textBufferSetFull(buffer);
			#endif

			str = &str[remainingBuffer];
			strLength -= remainingBuffer;
		}

		// Fill the buffer with the remaining bytes of the string.
		memcpy(buffer->cursor, str, strLength);
		buffer->cursor += strLength;
	}
}

// Read and return a single encoded UTF-8 character from the specified stream.
uint32_t textBufferRead(const textBuffer *const restrict buffer, const byte_t **const restrict cursor){
	textCMapCodeUnit_t character = {._32 = 0};

	character.byte._1 = readBufferByte(buffer, cursor);
	// The character uses one byte.
	if(character.byte._1 <= UTF8_1_BYTE_LIMIT){
		return(character._32);
	}else if(byteIsValid(character.byte._2 = readBufferByte(buffer, cursor))){
		// The character uses two bytes.
		if(character.byte._2 <= UTF8_2_BYTE_LIMIT){
			return(character._32);
		}else if(byteIsValid(character.byte._3 = readBufferByte(buffer, cursor))){
			// The character uses three bytes.
			if(character.byte._3 <= UTF8_3_BYTE_LIMIT){
				return(character._32);
			}else if(byteIsValid(character.byte._4 = readBufferByte(buffer, cursor))){
				// The character uses four bytes.
				if(character.byte._4 <= UTF8_4_BYTE_LIMIT){
					return(character._32);
				}
			}
		}
	}

	// None of the conditions were met, so the character is invalid.
	return(TEXT_UTF8_INVALID_CODE);
}


void textBufferDelete(textBuffer *const restrict buffer){
	memoryManagerGlobalFree(buffer->start);
}


/*
** Read the next byte in a text buffer and advance the cursor.
** We leave it up to the user to check for the end of the buffer.
*/
static byte_t readBufferByte(const textBuffer *const restrict buffer, const byte_t **const restrict cursor){
	if(*cursor != buffer->end){
		const byte_t curByte = **cursor;
		++(*cursor);

		return(curByte);
	}

	return(UTF8_INVALID_BYTE);
}