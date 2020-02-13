#ifndef textGlyph_h
#define textGlyph_h


#include "rectangle.h"
#include "texture.h"


typedef struct textGlyph {
	// This tells us where the character appears in the font image.
	rectangle uvOffsets;

	// How to position the glyph relative to others on the line.
	float kerningX;
	float kerningY;
	// How far to move the cursor after entering the glyph.
	float advanceX;
} textGlyph;


textGlyph *textGlyphArrayLoad(const char *const restrict glyphPath, const texture *const restrict atlas);


#endif