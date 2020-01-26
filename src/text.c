#include "text.h"


// By finding the smallest value from the following list
// that our character's first byte is less than, we can
// determine how many bytes that character should use.
#define UTF8_1_BYTE_LIMIT     0x007F
#define UTF8_2_BYTE_LIMIT     0x00DF
#define UTF8_3_BYTE_LIMIT     0x00EF
#define UTF8_4_BYTE_LIMIT     0x00FF
// Any extra bytes should be less than this value.
#define UTF8_EXTRA_BYTE_LIMIT 0x00BF

// We can use these masks to get the
// codepoints from the encoded bytes.
#define UTF8_1_BYTE_CODEPOINT_MASK     0x007F
#define UTF8_2_BYTE_CODEPOINT_MASK     0x001F
#define UTF8_3_BYTE_CODEPOINT_MASK     0x000F
#define UTF8_4_BYTE_CODEPOINT_MASK     0x0007
#define UTF8_EXTRA_BYTE_CODEPOINT_MASK 0x003F

// Bytes should begin with these header bits.
#define UTF8_1_BYTE_HEADER     0x0000
#define UTF8_2_BYTE_HEADER     0x00C0
#define UTF8_3_BYTE_HEADER     0x00E0
#define UTF8_4_BYTE_HEADER     0x00F0
#define UTF8_EXTRA_BYTE_HEADER 0x0080

// We can use these masks to validate the bytes' headers.
#define UTF8_1_BYTE_HEADER_MASK     0x0080
#define UTF8_2_BYTE_HEADER_MASK     0x00E0
#define UTF8_3_BYTE_HEADER_MASK     0x00F0
#define UTF8_4_BYTE_HEADER_MASK     0x00F8
#define UTF8_EXTRA_BYTE_HEADER_MASK 0x00C0

#define byteIsValid(byte) (((byte) & UTF8_EXTRA_BYTE_HEADER_MASK) == UTF8_EXTRA_BYTE_HEADER)


// Forward-declare any helper functions!
static fontCmapCodeUnit_t readCharUTF8();


// Read and return a single encoded UTF-8 character from the specified stream.
static fontCmapCodeUnit_t readCharUTF8(){
	fontCmapCodeUnit_t character = {._32 = 0};

	character.byte._1 = 0/*readByte()*/;
	// The character uses one byte.
	if(character.byte._1 <= UTF8_1_BYTE_LIMIT){
		return(character);
	}else if(byteIsValid(character.byte._2 = 0/*readByte()*/)){
		// The character uses two bytes.
		if(character.byte._1 <= UTF8_2_BYTE_LIMIT){
			return(character);
		}else if(byteIsValid(character.byte._3 = 0/*readByte()*/)){
			// The character uses three bytes.
			if(character.byte._1 <= UTF8_3_BYTE_LIMIT){
				return(character);
			}else if(byteIsValid(character.byte._4 = 0/*readByte()*/)){
				// The character uses four bytes.
				if(character.byte._1 <= UTF8_4_BYTE_LIMIT){
					return(character);
				}
			}
		}
	}

	// None of the conditions were met, so the character is invalid.
	character._32 = 0;

	return(character);
}