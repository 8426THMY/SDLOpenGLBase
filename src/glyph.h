#ifndef glyph_h
#define glyph_h


#include "rectangle.h"


typedef struct glyph {
	// This tells us where the character appears in the font image.
	rectangle uvOffsets;

	// How to position the glyph relative to others on the line.
	float kerningX;
	float kerningY;
	// How far to move the cursor after entering the glyph.
	float advanceX;
} glyph;


#endif