#include "text.h"


#include "memoryManager.h"


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
static byte_t readBufferByte(const textBuffer *const restrict text, const byte_t **const restrict cursor);


void textBufferInit(textBuffer *const restrict text, const size_t size){
	text->offset = text->start = text->end = memoryManagerGlobalAlloc(size);
	if(text->offset == NULL){
		/** MALLOC FAILED **/
	}
	text->end += size;
}


// Write a string to a text buffer!
#warning "This is temporary, I'm not sure how to do it properly in a nice way."
void textBufferWrite(textBuffer *const restrict text, char *str, size_t strSize){
	size_t curSize;

	// If the new string will overflow the buffer, write to the
	// end of the buffer and then start writing from the beginning.
	while((curSize = text->end - text->offset) > strSize){
		memcpy(text->offset, str, curSize);
		text->offset = text->start;
		str += curSize;
		strSize -= curSize;
	}

	// Write the remaining characters.
	memcpy(text->offset, str, strSize);
	//text->offset += strSize;
}

// Read and return a single encoded UTF-8 character from the specified stream.
uint32_t textBufferRead(const textBuffer *const restrict text, const byte_t **const restrict cursor){
	textCMapCodeUnit_t character = {._32 = 0};

	character.byte._1 = readBufferByte(text, cursor);
	// The character uses one byte.
	if(character.byte._1 <= UTF8_1_BYTE_LIMIT){
		return(character._32);
	}else if(byteIsValid(character.byte._2 = readBufferByte(text, cursor))){
		// The character uses two bytes.
		if(character.byte._2 <= UTF8_2_BYTE_LIMIT){
			return(character._32);
		}else if(byteIsValid(character.byte._3 = readBufferByte(text, cursor))){
			// The character uses three bytes.
			if(character.byte._3 <= UTF8_3_BYTE_LIMIT){
				return(character._32);
			}else if(byteIsValid(character.byte._4 = readBufferByte(text, cursor))){
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


void textBufferDelete(textBuffer *const restrict text){
	memoryManagerGlobalFree(text->start);
}


// Read the next byte in a text buffer and move the cursor.
#warning "This is temporary, I'm not sure how to do it properly in a nice way."
static byte_t readBufferByte(const textBuffer *const restrict text, const byte_t **const restrict cursor){
	/*if(*cursor == text->offset){
		return(UTF8_INVALID_BYTE);
	}else if(*cursor == text->end){
		*cursor = text->start;
	}
	++(*cursor);

	return(**cursor);*/
	if(*cursor != text->end){
		const byte_t curByte = **cursor;
		++(*cursor);

		return(curByte);
	}

	return(UTF8_INVALID_BYTE);
}