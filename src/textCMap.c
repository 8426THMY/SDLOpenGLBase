#include "textCMap.h"


#include <stddef.h>
#include <stdio.h>

#include "utilFile.h"

#include "memoryManager.h"


// Different formats are designed to only
// work for different ranges of code units.
#define TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT 0xFF
#define TEXT_CMAP_FORMAT_2_CODEUNIT_LIMIT 0xFFFF
#define TEXT_CMAP_FORMAT_4_CODEUNIT_LIMIT 0xFFFF

#define TEXT_CMAP_INVALID_CODEUNIT_ID 0xFFFFFFFF
#define TEXT_CMAP_MISSING_GLYPH_ID    0x00000000

#define TTF_HEAD_CHECKSUM_ADJUSTMENT_OFFSET (2*sizeof(uint16_t) + sizeof(uint32_t))

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
#define textCMapMod16(x) ((x) & 0x0000FFFF)

// Find a particular subheader using the high byte of a code unit.
#define textCMapFormat2GetSubHeader(cmap, high) (const textCMapSubHeader2 *)((const byte_t *)(&((const textCMapFormat2 *)(cmap))->data) + ((const textCMapFormat2 *)(cmap))->subHeaderKeys[high]);
// Find the glyph index for a format 2 subtable.
#define textCMapFormat2GetGlyphIndex(header, i) ((const uint16_t *)((const byte_t *)&((header)[1]) + (header)->idRangeOffset))[i];

// Get an element in the character map's end code array.
#define textCMapFormat4GetEndCodes(cmap)   ((const uint16_t *)(&((const textCMapFormat4 *)(cmap))->data))
#define textCMapFormat4GetEndCode(cmap, i) (const uint16_t *)((const byte_t *)textCMapFormat4GetEndCodes(cmap) + i)
// Get an element in the character map's start code array.
#define textCMapFormat4GetStartCodes(cmap)   ((const uint16_t *)((const byte_t *)&((const textCMapFormat4 *)(cmap))->data + ((const textCMapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define textCMapFormat4GetStartCode(cmap, i) (const uint16_t *)((const byte_t *)textCMapFormat4GetStartCodes(cmap) + i)
// Get an element in the character map's delta array.
#define textCMapFormat4GetDeltas(cmap)   ((const int16_t *)((const byte_t *)&((const textCMapFormat4 *)(cmap))->data + 2*((const textCMapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define textCMapFormat4GetDelta(cmap, i) (const int16_t *)((const byte_t *)textCMapFormat4GetDeltas(cmap) + i)
// Get an element in the character map's offset array.
#define textCMapFormat4GetOffsets(cmap)   ((const uint16_t *)((const byte_t *)&((const textCMapFormat4 *)(cmap))->data + 3*((const textCMapFormat4 *)(cmap))->segCountX2 + sizeof(uint16_t)))
#define textCMapFormat4GetOffset(cmap, i) (const uint16_t *)((const byte_t *)textCMapFormat4GetOffsets(cmap) + i)
// Find the glyph index for a format 4 subtable.
#define textCMapFormat4GetGlyphIndex(start, offset, code) *(((*(offset)) >> 1) + ((code) - (start)) + (offset))


// Forward-declare any helper functions!
static textCMapHeader *loadInvalid(FILE *const restrict file);
static textCMapHeader *loadFormat0(FILE *const restrict file);
static textCMapHeader *loadFormat2(FILE *const restrict file);
static textCMapHeader *loadFormat4(FILE *const restrict file);

static uint32_t calculateTableChecksum(FILE *const restrict file, const uint32_t tableOffset, const uint32_t tableLength);
static textCMapHeader *readCmapTable(FILE *const restrict file);

static uint32_t indexInvalid(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code);
static uint32_t indexFormat0(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code);
static uint32_t indexFormat2(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code);
static uint32_t indexFormat4(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code);


static textCMapHeader *(*textCMapLoadTable[TEXT_CMAP_NUM_SUPPORTED_FORMATS])(FILE *const restrict file) = {
	loadFormat0,
	loadInvalid,
	loadFormat2,
	loadInvalid,
	loadFormat4
};

uint32_t (*textCMapIndexTable[TEXT_CMAP_NUM_SUPPORTED_FORMATS])(
	const textCMapHeader *const restrict cmap,
	const textCMapCodeUnit_t code
) = {
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
textCMapHeader *textCMapLoadTTF(const char *const restrict ttfPath){
	FILE *ttfFile;

	ttfFile = fopen(ttfPath, "rb");
	if(ttfFile != NULL){
		uint16_t numTables;
		uint32_t cmapOffset = 0;
		textCMapHeader *cmap = NULL;

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
				fseek(ttfFile, offset + TTF_HEAD_CHECKSUM_ADJUSTMENT_OFFSET, SEEK_SET);
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
uint32_t textCMapIndex(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code){
	return(textCMapIndexTable[*cmap](cmap, code));
}

void textCMapDelete(textCMapHeader *const restrict cmap){
	memoryManagerGlobalFree(cmap);
}


// Output the code point of every supported character in a map to a text file!
#warning "This is temporary and should be removed."
void textCMapOutputCodePoints(const textCMapHeader *const restrict cmap, const char *const restrict filePath, const char delim){
	FILE *const outFile = fopen(filePath, "w");
	if(outFile != NULL){
		uint16_t i, j = 0;
		for(i = 0; i < 0xFFFF; ++i){
			const uint32_t index = textCMapIndex(cmap, (textCMapCodeUnit_t){._32 = i});
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
static textCMapHeader *loadInvalid(FILE *const restrict file){
	return(NULL);
}

static textCMapHeader *loadFormat0(FILE *const restrict file){
	const uint16_t length = readUint16LE(file);

	textCMapFormat0 *const cmap = memoryManagerGlobalAlloc(length);
	if(cmap == NULL){
		/** MALLOC FAILED **/
	}


	cmap->format   = 0;
	cmap->length   = length;
	cmap->language = readUint16LE(file);

	// Read the glyph indices into the character map's array.
	fread(cmap->glyphIndices, sizeof(*cmap->glyphIndices), 256, file);


	return((textCMapHeader *)cmap);
}

static textCMapHeader *loadFormat2(FILE *const restrict file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	const uint16_t *lastInt;

	textCMapFormat2 *const cmap = memoryManagerGlobalAlloc(length);
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


	return((textCMapHeader *)cmap);
}

static textCMapHeader *loadFormat4(FILE *const restrict file){
	const uint16_t length = readUint16LE(file);
	uint16_t *curInt;
	uint16_t *lastInt;

	textCMapFormat4 *const cmap = memoryManagerGlobalAlloc(length);
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


	return((textCMapHeader *)cmap);
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
static textCMapHeader *readCmapTable(FILE *const restrict file){
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
			textCMapHeader *(*const cmapLoadFunc)(FILE *const restrict file) = textCMapLoadTable[format];
			// If the subtable has a supported format, we can load the map!
			if(cmapLoadFunc != NULL){
				textCMapHeader *const cmap = (*cmapLoadFunc)(file);
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
static uint32_t indexInvalid(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code){
	return(TEXT_CMAP_MISSING_GLYPH_ID);
}

static uint32_t indexFormat0(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code){
	// Format 0 requires that characters be at most 1 byte.
	if(code._32 > TEXT_CMAP_FORMAT_0_CODEUNIT_LIMIT){
		return(TEXT_CMAP_MISSING_GLYPH_ID);
	}
	return(((textCMapFormat0 *)cmap)->glyphIndices[code._32]);
}

static uint32_t indexFormat2(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code){
	// Format 2 requires that characters be at most 2 bytes.
	if(code._32 <= TEXT_CMAP_FORMAT_2_CODEUNIT_LIMIT){
		const textCMapSubHeader2 *const subHeader = textCMapFormat2GetSubHeader(cmap, code.bytes[1]);
		const uint8_t subArrayIndex = code.bytes[0] - subHeader->firstCode;

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(subArrayIndex < subHeader->entryCount){
			const uint16_t glyphIndex = textCMapFormat2GetGlyphIndex(subHeader, subArrayIndex);
			if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
				return(textCMapMod16(glyphIndex + subHeader->idDelta));
			}
		}
	}

	return(TEXT_CMAP_MISSING_GLYPH_ID);
}

static uint32_t indexFormat4(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code){
	// Format 4 requires that characters be at most 2 bytes.
	if(code._32 <= TEXT_CMAP_FORMAT_4_CODEUNIT_LIMIT){
		const uint16_t *end = textCMapFormat4GetEndCodes(cmap);
		uint16_t start;

		uint16_t i;
		// Find the segment that maps our character.
		for(i = 0; *end < code._16; ++end, i += 2);
		start = *textCMapFormat4GetStartCode(cmap, i);

		// If the index is inside the subheader's range,
		// get the glyph associated with the character.
		if(code._16 >= start){
			const int16_t delta = *textCMapFormat4GetDelta(cmap, i);
			const uint16_t *const offset = textCMapFormat4GetOffset(cmap, i);
			// If the offset is 0, we should just add the delta to the code unit.
			if(*offset == 0){
				return(textCMapMod16(code._16 + delta));

			// Otherwise, we need to do a bit more work.
			}else{
				const uint16_t glyphIndex = textCMapFormat4GetGlyphIndex(start, offset, code._16);
				if(glyphIndex != TEXT_CMAP_MISSING_GLYPH_ID){
					return(textCMapMod16(glyphIndex + delta));
				}
			}
		}
	}

	return(TEXT_CMAP_MISSING_GLYPH_ID);
}