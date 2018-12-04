#include "moduleTexture.h"


memoryPool *textureManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTextureSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager
	//and the error object can be setup correctly.
	return(
		textureManager = memoryManagerGlobalAlloc(TEXTURE_BASE_MEMORY + sizeof(textureManager)) &&
		textureSetupError()
	);
}


//Allocate memory for a texture
//and return a handle to it.
texture *moduleTextureAlloc(){
	return(memPoolAlloc(textureManager));
}

//Free a texture that has been allocated.
void moduleTextureFree(texture *tex){
	textureDelete(tex);
	memPoolFree(textureManager, tex);
}


void moduleTextureCleanup(){
	/*size_t i = loadedTextures.size;
	//Loop through the texture manager.
	while(i > 0){
		--i;
		textureDelete((texture *)vectorGet(&loadedTextures, i));
	}*/
	memoryManagerGlobalFree(textureManager);
}