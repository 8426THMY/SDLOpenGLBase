#include "font.h"


#include "utilFile.h"

#include "moduleTexture.h"


return_t fontLoad(font *f, const char *texPath, const char *cmapPath){
	if(!(f->tex = moduleTextureAlloc())){
		/** MALLOC FAILED **/
	}
	// Load the font's graphics.
	f->tex = textureLoad(texPath);

	// Load the font's glyph information.
	//f->glyphs = glyphLoadArray(

	// Allocate memory for the font's character map and load it.
	f->cmap = charMapLoadTTF(cmapPath);


	return(0);
}


void fontDelete(font *f){
	charMapDelete(f->cmap);
}