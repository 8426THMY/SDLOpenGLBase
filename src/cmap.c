#include "cmap.h"


#include <stddef.h>
#include <stdio.h>

#include "utilFile.h"

#include "memoryManager.h"


// Different formats are designed to only
// work for different ranges of code units.
#define CMAP_FORMAT_0_CODEUNIT_LIMIT 0xFF
#define CMAP_FORMAT_2_CODEUNIT_LIMIT 0xFFFF
#define CMAP_FORMAT_4_CODEUNIT_LIMIT 0xFFFF

#define CMAP_INVALID_CODEUNIT_ID 0xFFFFFFFF
#define CMAP_MISSING_GLYPH_ID    0x00000000

#define CMAP_FORMAT_UNSUPPORTED 0
#define CMAP_FORMAT_SUPPORTED   1

#define TTF_HEAD_CHECKSUMADJUSTMENT_OFFSET (2*sizeof(uint16_t) + sizeof(uint32_t))

#define TTF_CMAP_PLATFORMID_UNICODE 0
#define TTF_CMAP_PLATFORMID_MAC     1
#define TTF_CMAP_PLATFORMID_ISO     2
#define TTF_CMAP_PLATFORMID_WINDOWS 3
#define TTF_CMAP_PLATFORMID_CUSTOM  4

#define TTF_CMAP_ENCODINGID_1_0      0
#define TTF_CMAP_ENCODINGID_1_1      1
#define TTF_CMAP_ENCODINGID_ISO      2
#define TTF_CMAP_ENCODINGID_2_0_BMP  3
#define TTF_CMAP_ENCODINGID_2_0_FULL 4
#define TTF_CMAP_ENCODINGID_VAR      5
#define TTF_CMAP_ENCODINGID_FULL     6

// Take the remainder of x modulo 65,536.
#define cmapMod16(x) ((x) & 0x0000FFFF)

// Find a particular subheader using the high byte of a code unit.
#define cmapFormat2GetSubHeader(cmap, high) (const f2SubHeader *)((const byte_t *)(&((const subtableFormat2 *)(cmap))->data) + ((const subtableFormat2 *)(cmap))->subHeaderKeys[high]);
// Find the glyph index for a format 2 subtable.
#define cmapFormat2GetGlyphIndex(header, i) ((const uint16_t *)((const byte_t *)&((header)->idRangeOffset) + sizeof((header)->idRangeOffset) + (header)->idRangeOffset))[i];

// Get an element in the character map's end code array.
#define cmapFormat4GetEndCodes(cmap)   ((const uint16_t *)(&((const subtableFormat4 *)(cmap))->data))
#define cmapFormat4GetEndCode(cmap, i) (const uint16_t *)((const byte_t *)cmapFormat4GetEndCodes(cmap) + i)
// Get an element in the character map's start code array.
#define cmapFormat4GetStartCodes(cmap)   ((const uint16_t *)((const byte_t *)&((const subtableFormat4 *)(cmap))->data + ((const subtableFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define cmapFormat4GetStartCode(cmap, i) (const uint16_t *)((const byte_t *)cmapFormat4GetStartCodes(cmap) + i)
// Get an element in the character map's delta array.
#define cmapFormat4GetDeltas(cmap)   ((const int16_t *)((const byte_t *)&((const subtableFormat4 *)(cmap))->data + 2*((const subtableFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define cmapFormat4GetDelta(cmap, i) (const int16_t *)((const byte_t *)cmapFormat4GetDeltas(cmap) + i)
// Get an element in the character map's offset array.
#define cmapFormat4GetOffsets(cmap)   ((const uint16_t *)((const byte_t *)&((const subtableFormat4 *)(cmap))->data + 3*((const subtableFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define cmapFormat4GetOffset(cmap, i) (const uint16_t *)((const byte_t *)cmapFormat4GetOffsets(cmap) + i)
// Find the glyph index for a format 4 subtable.
#define cmapFormat4GetGlyphIndex(start, offset, code) *(((*(offset)) >> 1) + ((code) - (start)) + (offset))


// Forward-declare any helper functions!
static cmapHeader *charMapLoadInvalid(FILE *file);
static cmapHeader *charMapLoadFormat0(FILE *file);
static cmapHeader *charMapLoadFormat2(FILE *file);
static cmapHeader *charMapLoadFormat4(FILE *file);

static uint32_t calculateTableChecksum(FILE *file, const uint32_t tableOffset, const uint32_t tableLength);
static cmapHeader *readCharMapTable(FILE *file);

static uint32_t charMapIndexInvalid(const cmapHeader *cmap, const charCodeUnit_t code);
static uint32_t charMapIndexFormat0(const cmapHeader *cmap, const charCodeUnit_t code);
static uint32_t charMapIndexFormat2(const cmapHeader *cmap, const charCodeUnit_t code);
static uint32_t charMapIndexFormat4(const cmapHeader *cmap, const charCodeUnit_t code);


static return_t charMapFormatSupported[CMAP_NUM_FORMATS] = {
	CMAP_FORMAT_SUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_SUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_SUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED,
	CMAP_FORMAT_UNSUPPORTED
};

static cmapHeader *(*charMapLoadJumpTable[CMAP_NUM_SUPPORTED_FORMATS])(FILE *file) = {
	charMapLoadFormat0,
	charMapLoadInvalid,
	charMapLoadFormat2,
	charMapLoadInvalid,
	charMapLoadFormat4
};

uint32_t (*charMapIndexJumpTable[CMAP_NUM_SUPPORTED_FORMATS])(const cmapHeader *cmap, const charCodeUnit_t code) = {
	charMapIndexFormat0,
	charMapIndexInvalid,
	charMapIndexFormat2,
	charMapIndexInvalid,
	charMapIndexFormat4
};


/*
** Load and return a character map from the file specified!
** If we couldn't load any character maps, NULL is returned.
*/
#warning "At the moment, this assumes that 'cmapPath' is the path to a TrueType font."
#warning "TrueType fonts are little-endian."
cmapHeader *charMapLoadTTF(const char *ttfPath){
	FILE *ttfFile;

	ttfFile = fopen(ttfPath, "rb");
	if(ttfFile != NULL){
		uint16_t numTables;
		uint32_t cmapOffset = 0;
		cmapHeader *cmap = NULL;

		// Load the font's offset table information.
		// We only really need the number of tables.
		readUint32LE(ttfFile); // scalerType
		numTables = readUint16LE(ttfFile);
		readUint16LE(ttfFile); // searchRange
		readUint16LE(ttfFile); // entrySelector
		readUint16LE(ttfFile); // rangeShift

		// Search the table directory for the "cmap" table.
		// We also need to check each subtable's checksum.
		for(; numTables > 0; --numTables){
			// Load the current table directory entry's information.
			const char tag[5]     = {readByte(ttfFile), readByte(ttfFile), readByte(ttfFile), readByte(ttfFile), '\0'};
			uint32_t checkSum     = readUint32LE(ttfFile);
			const uint32_t offset = readUint32LE(ttfFile);
			const uint32_t length = readUint32LE(ttfFile);

			checkSum -= calculateTableChecksum(ttfFile, offset, length);
			// If this is the head table, we need to add the value of
			// "checkSumAdjustment", since we accidentally removed it.
			if(strcmp(tag, "head") == 0){
				const long oldPos = ftell(ttfFile);
				fseek(ttfFile, offset + TTF_HEAD_CHECKSUMADJUSTMENT_OFFSET, SEEK_SET);
				checkSum += readUint32LE(ttfFile);
				fseek(ttfFile, oldPos, SEEK_SET);

			// Remember the character map's table offset for when
			// we've finished verifying the other tables' checksums.
			}else if(strcmp(tag, "cmap") == 0){
				cmapOffset = offset;
			}

			// If the table's checksum doesn't
			// match, stop trying to load the font.
			if(checkSum != 0){
				printf(
					"Checksum mismatch for font table %s!\n"
					"Path: %s\n",
					tag, ttfPath
				);
				fclose(ttfFile);

				return(NULL);
			}
		}

		// If we found a character map table, load its data!
		if(cmapOffset != 0){
			fseek(ttfFile, cmapOffset, SEEK_SET);
			cmap = readCharMapTable(ttfFile);
			if(cmap == NULL){
				printf(
					"Font file does not contain a supported character map format!\n"
					"Path: %s\n",
					ttfPath
				);
			}
		}else{
			printf(
				"Font file does not contain a character map table!\n"
				"Path: %s\n",
				ttfPath
			);
		}

		fclose(ttfFile);


		return(cmap);
	}else{
		printf(
			"Unable to open character map!\n"
			"Path: %s\n",
			ttfPath
		);
	}


	return(NULL);
}

// Given a code unit, return the index of the glyph associated with it.
uint32_t charMapIndex(const cmapHeader *cmap, const charCodeUnit_t code){
	return(charMapIndexJumpTable[*cmap](cmap, code));
}

void charMapDelete(cmapHeader *cmap){
	memoryManagerGlobalFree(cmap);
}


// This function is used if we try to load a character map with an unsupported format.
static cmapHeader *charMapLoadInvalid(FILE *file){
	return(NULL);
}

static cmapHeader *charMapLoadFormat0(FILE *file){
	const uint16_t length = readUint16LE(file);

	subtableFormat0 *cmap = memoryManagerGlobalAlloc(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format   = 0;
	cmap->length   = length;
	cmap->language = readUint16LE(file);

	// Read the glyph indices into the character map's array.
	fread(cmap->glyphIndices, sizeof(*cmap->glyphIndices), 256, file);


	return((cmapHeader *)cmap);
}

static cmapHeader *charMapLoadFormat2(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	const uint16_t *lastInt;

	subtableFormat2 *cmap = memoryManagerGlobalAlloc(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format = 2;
	cmap->length = length;

	curInt = &cmap->language;
	lastInt = (uint16_t *)((byte_t *)cmap + length);
	// Load the character map's data.
	for(; curInt < lastInt; ++curInt){
		*curInt = readUint16LE(file);
	}


	return((cmapHeader *)cmap);
}

static cmapHeader *charMapLoadFormat4(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	uint16_t *lastInt;

	subtableFormat4 *cmap = memoryManagerGlobalAlloc(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format = 4;
	cmap->length = length;

	curInt = &cmap->language;
	lastInt = (uint16_t *)((byte_t *)cmap + length);
	// Read the character map's segments and glyph indices.
	for(; curInt < lastInt; ++curInt){
		*curInt = readUint16LE(file);
	}


	return((cmapHeader *)cmap);
}


// Calculate the checksum for a TrueType font's offset table.
static uint32_t calculateTableChecksum(FILE *file, const uint32_t tableOffset, const uint32_t tableLength){
	uint32_t sum = 0;
	uint32_t numUint32s = ((tableLength + 3) & ~3) / sizeof(uint32_t);

	const int32_t oldPos = ftell(file);
	// Set the file pointer to the beginning of the table's data.
	fseek(file, tableOffset, SEEK_SET);

	// Sum the 32-bit integers in the table.
	for(; numUint32s > 0; --numUint32s){
		sum += readUint32LE(file);
	}

	// Return the file pointer to its original position.
	fseek(file, oldPos, SEEK_SET);


	return(sum);
}

/*
** Read through a font's character map table and
** load the first supported subtable we find!
*/
static cmapHeader *readCharMapTable(FILE *file){
	const long tableStart = ftell(file);
	uint16_t numTables;

	// Load the character map's table information.
	// We only really need the number of subtables.
	readUint16LE(file); // version
	numTables = readUint16LE(file);

	for(; numTables > 0; --numTables){
		const uint16_t platformID = readUint16LE(file);
		const uint16_t encodingID = readUint16LE(file);
		const uint32_t offset     = readUint32LE(file);

		// We only load character map subtables for the Unicode platform
		// that are limited to supporting the basic multilingual plane.
		if(platformID == TTF_CMAP_PLATFORMID_UNICODE && encodingID <= TTF_CMAP_ENCODINGID_2_0_BMP){
			const long oldPos = ftell(file);
			const uint16_t format = (fseek(file, tableStart + offset, SEEK_SET), readUint16LE(file));
			// If the subtable has a supported format, we can load the map!
			if(charMapFormatSupported[format]){
				cmapHeader *cmap = charMapLoadJumpTable[format](file);
				// Return the map if it was loaded successfully!
				if(cmap != NULL){
					return(cmap);
				}
			}
			fseek(file, oldPos, SEEK_SET);
		}
	}

	return(NULL);
}


// This function is used if we try to index a character map with an unsupported format.
static uint32_t charMapIndexInvalid(const cmapHeader *cmap, const charCodeUnit_t code){
	return(CMAP_MISSING_GLYPH_ID);
}

static uint32_t charMapIndexFormat0(const cmapHeader *cmap, const charCodeUnit_t code){
	// Format 0 requires that characters be at most 1 byte.
	if(code._32 > CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return(CMAP_MISSING_GLYPH_ID);
	}
	return(((subtableFormat0 *)cmap)->glyphIndices[code._32]);
}

static uint32_t charMapIndexFormat2(const cmapHeader *cmap, const charCodeUnit_t code){
	// Format 2 requires that characters be at most 2 bytes.
	if(code._32 <= CMAP_FORMAT_2_CODEUNIT_LIMIT){
		const f2SubHeader *subHeader = cmapFormat2GetSubHeader(cmap, code.byte._2);
		const uint8_t subArrayIndex = code.byte._1 - subHeader->firstCode;

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(subArrayIndex < subHeader->entryCount){
			const uint16_t glyphIndex = cmapFormat2GetGlyphIndex(subHeader, subArrayIndex);
			if(glyphIndex != CMAP_MISSING_GLYPH_ID){
				return(cmapMod16(glyphIndex + subHeader->idDelta));
			}
		}
	}

	return(CMAP_MISSING_GLYPH_ID);
}

static uint32_t charMapIndexFormat4(const cmapHeader *cmap, const charCodeUnit_t code){
	// Format 4 requires that characters be at most 2 bytes.
	if(code._32 <= CMAP_FORMAT_4_CODEUNIT_LIMIT){
		const uint16_t *end = cmapFormat4GetEndCodes(cmap);
		uint16_t start;

		uint16_t i;
		// Find the segment that maps our character.
		for(i = 0; *end < code._16; ++end, i += 2);
		start = *cmapFormat4GetStartCode(cmap, i);

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(code._16 >= start){
			const int16_t delta = *cmapFormat4GetDelta(cmap, i);
			const uint16_t *offset = cmapFormat4GetOffset(cmap, i);
			// If the offset is 0, we should just add the delta to the code unit.
			if(*offset == 0){
				return(cmapMod16(code._16 + delta));

			// Otherwise, we need to do a bit more work.
			}else{
				const uint16_t glyphIndex = cmapFormat4GetGlyphIndex(start, offset, code._16);
				if(glyphIndex != CMAP_MISSING_GLYPH_ID){
					return(cmapMod16(glyphIndex + delta));
				}
			}
		}
	}

	return(CMAP_MISSING_GLYPH_ID);
}