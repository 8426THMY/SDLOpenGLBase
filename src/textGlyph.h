#ifndef textGlyph_h
#define textGlyph_h


#include "rectangle.h"
#include "texture.h"


typedef struct textGlyph {
	// Pointer to the texture atlas containing this glyph.
	const texture *atlas;
	// Coordinates of the glyph in the atlas.
	rectangle uvOffsets;

	// How to position the glyph relative to others on the line.
	float kerningX;
	float kerningY;
	// How far to move the cursor after entering the glyph.
	float advanceX;
} textGlyph;


textGlyph *textGlyphArrayLoad(const char *const restrict glyphPath, const texture **const restrict atlasArray);


#endif