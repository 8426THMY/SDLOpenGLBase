#ifndef textFont_h
#define textFont_h


#include <stdint.h>

#include "texture.h"

#include "textGlyph.h"
#include "textCMap.h"


typedef uint_least8_t textFontType_t;

typedef struct textFont {
	// This actually points to a complete character map, but we
	// treat it as a pointer to the header to keep things generic.
	textCMapHeader *cmap;
	// Array of glyphs associated with the font.
	textGlyph *glyphs;

	textFontType_t type;
} textFont;


return_t textFontLoad(
	textFont *const restrict font, const textFontType_t type, const texture **const restrict atlasArray,
	const char *const restrict cmapPath, const char *const restrict glyphPath
);
const textGlyph *textFontGetGlyph(const textFont *const restrict font, const textCMapCodeUnit_t code);
void textFontDelete(textFont *const restrict font);


#endif