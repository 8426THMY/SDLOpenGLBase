#include "font.h"


#include "memoryManager.h"


return_t fontLoad(font *f, const type_t type, const char *texPath, const char *glyphPath, const char *cmapPath){
	// Load the font's graphics.
	f->atlas = textureLoad(texPath);
	f->type = type;

	// Load the font's glyph offset information.
	f->glyphs = fontGlyphArrayLoad(glyphPath);
	// Allocate memory for the font's character map and load it.
	f->cmap = fontCmapLoadTTF(cmapPath);


	return(1);
}


void fontDelete(font *f){
	memoryManagerGlobalDelete(f->glyphs);
	fontCmapDelete(f->cmap);
}