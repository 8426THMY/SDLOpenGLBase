#include "fontCmap.h"


#include <stddef.h>
#include <stdio.h>

#include "utilFile.h"

#include "memoryManager.h"


// Different formats are designed to only
// work for different ranges of code units.
#define FONT_CMAP_FORMAT_0_CODEUNIT_LIMIT 0xFF
#define FONT_CMAP_FORMAT_2_CODEUNIT_LIMIT 0xFFFF
#define FONT_CMAP_FORMAT_4_CODEUNIT_LIMIT 0xFFFF

#define FONT_CMAP_INVALID_CODEUNIT_ID 0xFFFFFFFF
#define FONT_CMAP_MISSING_GLYPH_ID    0x00000000

#define FONT_CMAP_FORMAT_UNSUPPORTED 0
#define FONT_CMAP_FORMAT_SUPPORTED   1

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
#define fontCmapMod16(x) ((x) & 0x0000FFFF)

// Find a particular subheader using the high byte of a code unit.
#define fontCmapFormat2GetSubHeader(cmap, high) (const fontCmapSubHeader2 *)((const byte_t *)(&((const fontCmapFormat2 *)(cmap))->data) + ((const fontCmapFormat2 *)(cmap))->subHeaderKeys[high]);
// Find the glyph index for a format 2 subtable.
#define fontCmapFormat2GetGlyphIndex(header, i) ((const uint16_t *)((const byte_t *)&((header)[1]) + (header)->idRangeOffset))[i];

// Get an element in the character map's end code array.
#define fontCmapFormat4GetEndCodes(cmap)   ((const uint16_t *)(&((const fontCmapFormat4 *)(cmap))->data))
#define fontCmapFormat4GetEndCode(cmap, i) (const uint16_t *)((const byte_t *)fontCmapFormat4GetEndCodes(cmap) + i)
// Get an element in the character map's start code array.
#define fontCmapFormat4GetStartCodes(cmap)   ((const uint16_t *)((const byte_t *)&((const fontCmapFormat4 *)(cmap))->data + ((const fontCmapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define fontCmapFormat4GetStartCode(cmap, i) (const uint16_t *)((const byte_t *)fontCmapFormat4GetStartCodes(cmap) + i)
// Get an element in the character map's delta array.
#define fontCmapFormat4GetDeltas(cmap)   ((const int16_t *)((const byte_t *)&((const fontCmapFormat4 *)(cmap))->data + 2*((const fontCmapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define fontCmapFormat4GetDelta(cmap, i) (const int16_t *)((const byte_t *)fontCmapFormat4GetDeltas(cmap) + i)
// Get an element in the character map's offset array.
#define fontCmapFormat4GetOffsets(cmap)   ((const uint16_t *)((const byte_t *)&((const fontCmapFormat4 *)(cmap))->data + 3*((const fontCmapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define fontCmapFormat4GetOffset(cmap, i) (const uint16_t *)((const byte_t *)fontCmapFormat4GetOffsets(cmap) + i)
// Find the glyph index for a format 4 subtable.
#define fontCmapFormat4GetGlyphIndex(start, offset, code) *(((*(offset)) >> 1) + ((code) - (start)) + (offset))


// Forward-declare any helper functions!
static fontCmapHeader *loadInvalid(FILE *file);
static fontCmapHeader *loadFormat0(FILE *file);
static fontCmapHeader *loadFormat2(FILE *file);
static fontCmapHeader *loadFormat4(FILE *file);

static uint32_t calculateTableChecksum(FILE *file, const uint32_t tableOffset, const uint32_t tableLength);
static fontCmapHeader *readCmapTable(FILE *file);

static uint32_t indexInvalid(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code);
static uint32_t indexFormat0(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code);
static uint32_t indexFormat2(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code);
static uint32_t indexFormat4(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code);


static return_t fontCmapFormatSupported[FONT_CMAP_NUM_FORMATS] = {
	FONT_CMAP_FORMAT_SUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_SUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_SUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED,
	FONT_CMAP_FORMAT_UNSUPPORTED
};

static fontCmapHeader *(*fontCmapLoadJumpTable[FONT_CMAP_NUM_SUPPORTED_FORMATS])(FILE *file) = {
	loadFormat0,
	loadInvalid,
	loadFormat2,
	loadInvalid,
	loadFormat4
};

uint32_t (*fontCmapIndexJumpTable[FONT_CMAP_NUM_SUPPORTED_FORMATS])(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code) = {
	indexFormat0,
	indexInvalid,
	indexFormat2,
	indexInvalid,
	indexFormat4
};


/*
** Load and return a character map from the file specified!
** If we couldn't load any character maps, NULL is returned.
*/
#warning "At the moment, this assumes that 'cmapPath' is the path to a TrueType font."
#warning "TrueType fonts are little-endian."
fontCmapHeader *fontCmapLoadTTF(const char *ttfPath){
	FILE *ttfFile;

	ttfFile = fopen(ttfPath, "rb");
	if(ttfFile != NULL){
		uint16_t numTables;
		uint32_t cmapOffset = 0;
		fontCmapHeader *cmap = NULL;

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
			cmap = readCmapTable(ttfFile);
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
uint32_t fontCmapIndex(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code){
	return(fontCmapIndexJumpTable[*cmap](cmap, code));
}

void fontCmapDelete(fontCmapHeader *cmap){
	memoryManagerGlobalFree(cmap);
}


// Output the code point of every supported character in a map to a text file!
#warning "This is temporary and should be removed."
void fontCmapOutputCodePoints(const fontCmapHeader *cmap, const char *filePath, const char delim){
	FILE *outFile = fopen(filePath, "w");
	if(outFile != NULL){
		uint16_t i, j = 0;
		for(i = 0; i < 0xFFFF; ++i){
			const uint32_t index = fontCmapIndex(cmap, (fontCmapCodeUnit_t){._32 = i});
			if(index != 0){
				fprintf(outFile, "%u%c", i, delim);
				++j;
			}
		}
		fclose(outFile);

		printf("Total mapped: %u\n", j);
	}else{
		printf(
			"Unable to open code point output file for reading!\n"
			"Path: %s\n",
			filePath
		);
	}
}


// This function is used if we try to load a character map with an unsupported format.
static fontCmapHeader *loadInvalid(FILE *file){
	return(NULL);
}

static fontCmapHeader *loadFormat0(FILE *file){
	const uint16_t length = readUint16LE(file);

	fontCmapFormat0 *cmap = memoryManagerGlobalAlloc(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format   = 0;
	cmap->length   = length;
	cmap->language = readUint16LE(file);

	// Read the glyph indices into the character map's array.
	fread(cmap->glyphIndices, sizeof(*cmap->glyphIndices), 256, file);


	return((fontCmapHeader *)cmap);
}

static fontCmapHeader *loadFormat2(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	const uint16_t *lastInt;

	fontCmapFormat2 *cmap = memoryManagerGlobalAlloc(length);
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


	return((fontCmapHeader *)cmap);
}

static fontCmapHeader *loadFormat4(FILE *file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	uint16_t *lastInt;

	fontCmapFormat4 *cmap = memoryManagerGlobalAlloc(length);
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


	return((fontCmapHeader *)cmap);
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
static fontCmapHeader *readCmapTable(FILE *file){
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
			if(fontCmapFormatSupported[format]){
				fontCmapHeader *cmap = fontCmapLoadJumpTable[format](file);
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
static uint32_t indexInvalid(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code){
	return(FONT_CMAP_MISSING_GLYPH_ID);
}

static uint32_t indexFormat0(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code){
	// Format 0 requires that characters be at most 1 byte.
	if(code._32 > FONT_CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return(FONT_CMAP_MISSING_GLYPH_ID);
	}
	return(((fontCmapFormat0 *)cmap)->glyphIndices[code._32]);
}

static uint32_t indexFormat2(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code){
	// Format 2 requires that characters be at most 2 bytes.
	if(code._32 <= FONT_CMAP_FORMAT_2_CODEUNIT_LIMIT){
		const fontCmapSubHeader2 *subHeader = fontCmapFormat2GetSubHeader(cmap, code.byte._2);
		const uint8_t subArrayIndex = code.byte._1 - subHeader->firstCode;

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(subArrayIndex < subHeader->entryCount){
			const uint16_t glyphIndex = fontCmapFormat2GetGlyphIndex(subHeader, subArrayIndex);
			if(glyphIndex != FONT_CMAP_MISSING_GLYPH_ID){
				return(fontCmapMod16(glyphIndex + subHeader->idDelta));
			}
		}
	}

	return(FONT_CMAP_MISSING_GLYPH_ID);
}

static uint32_t indexFormat4(const fontCmapHeader *cmap, const fontCmapCodeUnit_t code){
	// Format 4 requires that characters be at most 2 bytes.
	if(code._32 <= FONT_CMAP_FORMAT_4_CODEUNIT_LIMIT){
		const uint16_t *end = fontCmapFormat4GetEndCodes(cmap);
		uint16_t start;

		uint16_t i;
		// Find the segment that maps our character.
		for(i = 0; *end < code._16; ++end, i += 2);
		start = *fontCmapFormat4GetStartCode(cmap, i);

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(code._16 >= start){
			const int16_t delta = *fontCmapFormat4GetDelta(cmap, i);
			const uint16_t *offset = fontCmapFormat4GetOffset(cmap, i);
			// If the offset is 0, we should just add the delta to the code unit.
			if(*offset == 0){
				return(fontCmapMod16(code._16 + delta));

			// Otherwise, we need to do a bit more work.
			}else{
				const uint16_t glyphIndex = fontCmapFormat4GetGlyphIndex(start, offset, code._16);
				if(glyphIndex != FONT_CMAP_MISSING_GLYPH_ID){
					return(fontCmapMod16(glyphIndex + delta));
				}
			}
		}
	}

	return(FONT_CMAP_MISSING_GLYPH_ID);
}