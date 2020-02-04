#include "textFont.h"


#include "memoryManager.h"


return_t textFontLoad(textFont *font, const type_t type, const char *texPath, const char *glyphPath, const char *cmapPath){
	// Load the font's graphics.
	font->atlas = textureLoad(texPath);
	font->type = type;

	// Allocate memory for the font's character map and load it.
	font->cmap = textCmapLoadTTF(cmapPath);
	// Load the font's glyph offset information.
	font->glyphs = textGlyphArrayLoad(glyphPath, font->atlas);


	return(1);
}

const textGlyph *textFontGetGlyph(const textFont *font, const textCmapCodeUnit_t code){
	return(&font->glyphs[textCmapIndex(font->cmap, code)]);
}

void textFontDelete(textFont *font){
	textCmapDelete(font->cmap);
	memoryManagerGlobalFree(font->glyphs);
}