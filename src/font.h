#ifndef font_h
#define font_h


#include <stdint.h>

#include "texture.h"

#include "fontGlyph.h"
#include "fontCmap.h"


#define FONT_IMAGE_TYPE_NORMAL 0
#define FONT_IMAGE_TYPE_SDF    1
#define FONT_IMAGE_TYPE MSDF   2


typedef struct font {
	// We generally use simple bitmaps or signed distance fields
	// for our fonts, so we store the graphics in a texture atlas.
	texture *atlas;
	type_t type;

	// Array of glyphs associated with the font.
	fontGlyph *glyphs;
	// This actually points to a complete character map, but we
	// treat it as a pointer to the header to keep things generic.
	fontCmapHeader *cmap;
} font;


return_t fontLoad(font *f, const type_t type, const char *texPath, const char *glyphPath, const char *ttfPath);
void fontDelete(font *f);


#endif