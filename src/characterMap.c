#include "characterMap.h"


#include <stddef.h>


// Forward-declare any helper functions!
static uint32_t charMapIndexFormat0(const characterMap *cmap, const charCodeUnit_t code);
static uint32_t charMapIndexFormat2(const characterMap *cmap, const charCodeUnit_t code);
static uint32_t charMapIndexFormat4(const characterMap *cmap, const charCodeUnit_t code);


uint32_t (*charMapIndexJumpTable[CMAP_NUM_FORMATS])(const characterMap *cmap, const charCodeUnit_t code) = {
	charMapIndexFormat0,
	charMapIndexFormat2,
	charMapIndexFormat4
};


uint32_t charMapIndex(const characterMap *cmap, const charCodeUnit_t code){
	return(charMapIndexJumpTable[cmap->format](cmap, code));
}


static uint32_t charMapIndexFormat0(const characterMap *cmap, const charCodeUnit_t code){
	// Format 0 requires that characters be at most 1 byte.
	if(code._32 > CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return(CMAP_MISSING_GLYPH_ID);
	}
	return(((subTableFormat0 *)cmap)->glyphIndices[code._32]);
}

static uint32_t charMapIndexFormat2(const characterMap *cmap, const charCodeUnit_t code){
	// Format 2 requires that characters be at most 2 bytes.
	if(code._32 > CMAP_FORMAT_2_CODEUNIT_LIMIT){
		return(CMAP_MISSING_GLYPH_ID);
	}else{
		f2SubHeader subHeader;
		const uint16_t *subArray;
		uint8_t subArrayIndex;

		// If we're processing a two-byte character, find
		// the correct sub-header using the first byte.
		if(code.byte._2 != 0){
			subHeader = ((subTableFormat2 *)cmap)->subHeaders[((subTableFormat2 *)cmap)->subHeaderKeys[code.byte._1]];
			subArray = (uint16_t *)(((byte_t *)((subTableFormat2 *)cmap)->glyphIndices) + subHeader.idRangeOffset);
			subArrayIndex = code.byte._2 - subHeader.firstCode;

		// Otherwise, for single-byte characters, use sub-header 0.
		}else{
			subHeader = ((subTableFormat2 *)cmap)->subHeaders[0];
			subArray = (uint16_t *)(((byte_t *)((subTableFormat2 *)cmap)->glyphIndices) + subHeader.idRangeOffset);
			subArrayIndex = code.byte._1 - subHeader.firstCode;
		}

		// If the index is outside the sub-header's range, the
		// character cannot have any glyph associated with it.
		if(subArrayIndex >= subHeader.entryCount){
			return(CMAP_MISSING_GLYPH_ID);

		// Otherwise, return the character's glyph index.
		}else{
			uint16_t glyphIndex = subArray[subArrayIndex];
			if(glyphIndex != CMAP_MISSING_GLYPH_ID){
				glyphIndex += subHeader.idDelta;
			}
			return(glyphIndex);
		}
	}
}

static uint32_t charMapIndexFormat4(const characterMap *cmap, const charCodeUnit_t code){
	// Format 4 requires that characters be at most 2 bytes.
	if(code._32 > CMAP_FORMAT_4_CODEUNIT_LIMIT){
		return(CMAP_MISSING_GLYPH_ID);
	}else{
		const uint16_t *end = ((subTableFormat4 *)cmap)->endCode;
		uint16_t start;
		int16_t delta;
		uint16_t *offset;

		size_t segmentIndex = 0;
		// Find the segment that maps our character.
		while(*end < code._16){
			++end;
			++segmentIndex;
		}
		start = ((subTableFormat4 *)cmap)->startCode[segmentIndex];
		delta = ((subTableFormat4 *)cmap)->idDelta[segmentIndex];
		offset = &(((subTableFormat4 *)cmap)->idRangeOffset[segmentIndex]);

		// If the index is outside the sub-header's range, the
		// character cannot have any glyph associated with it.
		if(code._16 <= start){
			return(CMAP_MISSING_GLYPH_ID);

		// Otherwise, return the character's glyph index.
		}else{
			uint16_t glyphIndex = *(((*offset) >> 2) + (code._16 - start) + offset);
			if(glyphIndex != CMAP_MISSING_GLYPH_ID){
				glyphIndex += delta;
			}
			return(glyphIndex);
		}
	}
}