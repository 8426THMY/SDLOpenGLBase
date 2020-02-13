#include "textFont.h"


#include "memoryManager.h"


return_t textFontLoad(
	textFont *const restrict font, const textFontType_t type,
	const char *const restrict texPath, const char *const restrict glyphPath, const char *const restrict cmapPath
){

	// Load the font's graphics.
	font->atlas = textureLoad(texPath, strlen(texPath));
	font->type = type;

	// Allocate memory for the font's character map and load it.
	font->cmap = textCMapLoadTTF(cmapPath);
	// Load the font's glyph offset information.
	font->glyphs = textGlyphArrayLoad(glyphPath, font->atlas);


	return(1);
}

const textGlyph *textFontGetGlyph(const textFont *const restrict font, const textCMapCodeUnit_t code){
	return(&font->glyphs[textCMapIndex(font->cmap, code)]);
}

void textFontDelete(textFont *const restrict font){
	textCMapDelete(font->cmap);
	memoryManagerGlobalFree(font->glyphs);
}