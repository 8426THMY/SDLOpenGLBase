#ifndef font_h
#define font_h


#include "texture.h"

#include "glyph.h"
#include "cmap.h"


typedef struct font {
	// We generally use simple bitmaps or signed distance fields
	// for our fonts, so we store the graphics in a texture.
	texture *tex;

	// Array of glyphs associated with the font.
	glyph *glyphs;
	// This actually points to a complete character map,
	// but we treat it as a pointer to the header until
	// we know the subtable format.
	cmapHeader *cmap;
} font;


return_t fontLoad(font *f, const char *texPath, const char *ttfPath);
void fontDelete(font *f);


#endif