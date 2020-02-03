#ifndef textFont_h
#define textFont_h


#include <stdint.h>

#include "texture.h"

#include "textGlyph.h"
#include "textCmap.h"


#define TEXT_FONT_IMAGE_TYPE_NORMAL 0
#define TEXT_FONT_IMAGE_TYPE_SDF    1
#define TEXT_FONT_IMAGE_TYPE MSDF   2


typedef struct textFont {
	// We generally use simple bitmaps or signed distance fields
	// for our fonts, so we store the graphics in a texture atlas.
	texture *atlas;
	type_t type;

	// Array of glyphs associated with the font.
	textGlyph *glyphs;
	// This actually points to a complete character map, but we
	// treat it as a pointer to the header to keep things generic.
	textCmapHeader *cmap;
} textFont;


return_t textFontLoad(textFont *font, const type_t type, const char *texPath, const char *glyphPath, const char *ttfPath);
const textGlyph *textFontGetGlyph(const textFont *font, const textCmapCodeUnit_t code);
void textFontDelete(textFont *font);


#endif