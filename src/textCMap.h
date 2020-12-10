#ifndef textCMap_h
#define textCMap_h


#include <stdint.h>


// Since we only really use the basic multilingual plane
// at the moment, we only support formats 0, 2 and 4.
#define TEXT_CMAP_NUM_SUPPORTED_FORMATS 5
// This is the total number of formats.
#define TEXT_CMAP_NUM_FORMATS 15


// This structure is handy for reading
// and operating on encoded code points.
typedef union textCMapCodeUnit_t {
	uint8_t _8;
	uint16_t _16;
	uint32_t _32;
	struct {
		uint8_t _1;
		uint8_t _2;
		uint8_t _3;
		uint8_t _4;
	} byte;
} textCMapCodeUnit_t;


// All character maps share this header, so we can use it
// to represent the map until we know the subtable format.
typedef uint16_t textCMapHeader;


// Format 0 should be used for fonts
// that only use single-byte code units.
typedef struct textCMapFormat0 {
	textCMapHeader format;
	uint16_t length;
	uint16_t language;

	// This array maps from a code unit between
	// 0-255 to the index of its associated glyph.
	uint8_t glyphIndices[256];
} textCMapFormat0;


// Subheaders map the second byte of code
// units to a particular glyph index subarray.
typedef struct textCMapSubHeader2 {
	// These two values specify a range of values that we can
	// map to glyph subarrays. If the value of the second byte
	// of the code unit is within this range, then we use its
	// offset from firstCode to index into a glyph index subarray.
	// Otherwise, we map the character to the missing glyph.
	uint16_t firstCode;
	uint16_t entryCount;
	// For non-zero glyph indices, we need to add this to
	// get the actual glyph index. This means we can use
	// the same glyph subarray for multiple subheaders.
	int16_t idDelta;
	// This value represents the offset in bytes from
	// its own location in memory to the location of
	// the glyph subarray associated with this subheader.
	uint16_t idRangeOffset;
} textCMapSubHeader2;

/*
** Format 2 should be used for fonts that use a
** mixture of single-byte and two-byte code units.
**
**
** The data element contains memory for the following elements:
**
** // Array of subheaders that map to a subrange of glyph indices.
** f2Subheader *subHeaders;
**
** // Two-dimensional array of glyph indices, where the length of
** // the subarrays are given by their subheaders' entry counts.
** // The first subarray should map the single-byte characters.
** uint16_t **glyphIndices;
*/
typedef struct textCMapFormat2 {
	textCMapHeader format;
	uint16_t length;
	uint16_t language;

	// This maps the first byte of a code unit to a
	// particular subheader. If the code unit has only
	// a single byte, then we should use subheader 0.
	//
	// Note that we store the indices as an offset in bytes.
	uint16_t subHeaderKeys[256];

	// This is a uint16_t so we can begin loading from language.
	// Otherwise, if we use a void pointer, we get padding.
	uint16_t data;
} textCMapFormat2;


/*
** Format 4 is generally the standard format for
** fonts supporting the basic multilingual plane.
**
**
** The data element contains memory for the following elements:
**
** // Array of end codes, one per segment, that specify
** // the ending code units for a subrange. The last
** // segment should always map the code unit 0xFFFF.
** uint16_t *endCode;
**
** // This is also required by the file format for
** // easy loading. It should always be set to 0.
** uint16_t reservedPad;
**
** // Array of start codes, one per segment, that specify
** // the starting code units for a subrange.
** uint16_t *startCode;
**
** // Array of deltas, one per segment.
** // If idRangeOffset is 0, we add this value to
** // the character code to get the glyph index.
** int16_t *idDelta;
**
** // Array of offsets, one per segment.
** // This value represents the offset in bytes from its
** // own location in memory to the location of the range
** // of glyph indices associated with its segment.
** uint16_t *idRangeOffset;
*/
#warning "Can we remove the useless stuff and use an array of structures instead of a structure of arrays?"
#warning "We'd need to change the value of idRangeOffset for the latter, though..."
typedef struct textCMapFormat4 {
	textCMapHeader format;
	uint16_t length;
	uint16_t language;

	// This allows us to index the segment arrays.
	uint16_t segCountX2;
	// These are absolutely useless for us, but we
	// need to specify them at the moment so we can
	// load from our file more easily.
	uint16_t searchRange;
	uint16_t entrySelector;
	uint16_t rangeShift;

	// This is a uint16_t so we can begin loading from language.
	// Otherwise, if we use a void pointer, we get padding.
	uint16_t data;
} textCMapFormat4;


textCMapHeader *textCMapLoadTTF(const char *const restrict cmapPath);
uint32_t textCMapIndex(const textCMapHeader *const restrict cmap, const textCMapCodeUnit_t code);
void textCMapDelete(textCMapHeader *const restrict cmap);

#warning "This is temporary and should be removed."
void textCMapOutputCodePoints(const textCMapHeader *const restrict cmap, const char *const restrict filePath, const char delim);


extern uint32_t (*textCMapIndexTable[TEXT_CMAP_NUM_SUPPORTED_FORMATS])(
	const textCMapHeader *const restrict cmap,
	const textCMapCodeUnit_t code
);


#endif