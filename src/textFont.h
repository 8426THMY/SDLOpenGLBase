#ifndef textFont_h
#define textFont_h


#include <stdint.h>

#include "texture.h"

#include "textGlyph.h"
#include "textCMap.h"


#define TEXT_FONT_IMAGE_TYPE_NORMAL 0
#define TEXT_FONT_IMAGE_TYPE_SDF    1
#define TEXT_FONT_IMAGE_TYPE MSDF   2


typedef uint_least8_t textFontType_t;

typedef struct textFont {
	// We generally use simple bitmaps or signed distance fields
	// for our fonts, so we store the graphics in a texture atlas.
	texture *atlas;
	textFontType_t type;

	// This actually points to a complete character map, but we
	// treat it as a pointer to the header to keep things generic.
	textCMapHeader *cmap;
	// Array of glyphs associated with the font.
	textGlyph *glyphs;
} textFont;


return_t textFontLoad(
	textFont *const restrict font, const textFontType_t type,
	const char *const restrict texPath, const char *const restrict glyphPath, const char *const restrict ttfPath
);
const textGlyph *textFontGetGlyph(const textFont *const restrict font, const textCMapCodeUnit_t code);
void textFontDelete(textFont *const restrict font);


#endif