#ifndef characterMap_h
#define characterMap_h


#include <stdint.h>

#include "utilTypes.h"


#define CMAP_INVALID_CODEUNIT_ID 0xFFFFFFFF
#define CMAP_MISSING_GLYPH_ID    0x00000000

// Since we only really use the basic multilingual plane
// at the moment, we only support formats 0, 2 and 4.
#define CMAP_NUM_FORMATS 3

// Different formats are designed to only
// work for different ranges of code units.
#define CMAP_FORMAT_0_CODEUNIT_LIMIT 0xFF
#define CMAP_FORMAT_2_CODEUNIT_LIMIT 0xFFFF
#define CMAP_FORMAT_4_CODEUNIT_LIMIT 0xFFFF


// This structure is handy for reading
// and operating on encoded code points.
typedef union charCodeUnit_t {
	uint8_t _8;
	uint16_t _16;
	uint32_t _32;
	struct {
		uint8_t _1;
		uint8_t _2;
		uint8_t _3;
		uint8_t _4;
	} byte;
} charCodeUnit_t;


typedef type_t characterMapType_t;


// Format 0 should be used for fonts
// that only use single-byte code units.
typedef struct subTableFormat0 {
	characterMapType_t format;

	// This array maps from a code unit between
	// 0-255 to the index of its associated glyph.
	uint8_t glyphIndices[256];
} subTableFormat0;


// Sub-headers map the second byte of code
// units to a particular glyph index sub-array.
typedef struct f2SubHeader {
	// These two values specify a range of values that we can
	// map to glyph sub-arrays. If the value of the second byte
	// of the code unit is within this range, then we use its
	// offset from firstCode to index into a glyph index sub-array.
	// Otherwise, we map the character to the missing glyph.
	uint8_t firstCode;
	uint8_t entryCount;
	// For non-zero glyph indices, we need to add this to
	// get the actual glyph index. This means we can use
	// the same sub-array for multiple sub-headers.
	int16_t idDelta;
	// This value represents the offset in bytes from its
	// own location in memory to the location of the sub-array
	// associated with this sub-header.
	uint16_t idRangeOffset;
} f2SubHeader;

// Format 2 should be used for fonts that use a
// mixture of single-byte and two-byte code units.
typedef struct subTableFormat2 {
	characterMapType_t format;

	// This maps the first byte of a code unit to a
	// particular sub-header. If the code unit has only
	// a single byte, then we should use sub-header 0.
	uint8_t subHeaderKeys[256];
	f2SubHeader *subHeaders;

	// Two-dimensional array of glyph indices. The length of the
	// sub-arrays are given by their sub-header's entry counts.
	// The first sub-array should map the single-byte characters.
	uint16_t **glyphIndices;
} subTableFormat2;


// Format 4 is generally the standard format for
// fonts supporting the basic multilingual plane.
typedef struct subTableFormat4 {
	characterMapType_t format;

	// These arrays describe the segments that the
	// code units are divided into. Segments are
	// stored in order of increasing end codes.
	#warning "Shouldn't we use an array of structures?"
	// The start and end codes define the range
	// of character codes that the segment maps.
	// The last segment must always map 0xFFFF.
	uint16_t *endCode;
	uint16_t *startCode;
	// If idRangeOffset is 0, we add this value to
	// the character code to get the glyph index.
	int16_t *idDelta;
	// This value represents the offset in bytes from its
	// own location in memory to the location of the range
	// of glyph indices associated with its segment.
	uint16_t *idRangeOffset;

	// This array is segmented into ranges of contiguous
	// glyph indices. A single segment is hence capable
	// of mapping to one or more glyph indicies.
	uint16_t *glyphIndices;
} subTableFormat4;


// This is a generic character map type that's
// used to dereference any other until we know
// what format its subtable is in.
typedef struct characterMap {
	characterMapType_t format;
} characterMap;


uint32_t charMapIndex(const characterMap *cmap, const charCodeUnit_t code);


extern uint32_t (*charMapIndexJumpTable[CMAP_NUM_FORMATS])(
	const characterMap *cmap,
	const charCodeUnit_t code
);


#endif