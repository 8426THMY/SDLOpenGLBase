#include "moduleTexture.h"


#include "memoryManager.h"


// texture
moduleDefinePool(Texture, texture, g_textureManager, MODULE_TEXTURE_MANAGER_SIZE)
moduleDefinePoolFreeFlexible(Texture, texture, g_textureManager, textureDelete)

// Find a texture whose name matches the one specified!
texture *moduleTextureFind(const char *const restrict name){
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_textureManager, i)

	return(&g_texDefault);
}


return_t moduleTextureSetup(){
	return(moduleTextureInit() && textureSetup());
}

void moduleTextureCleanup(){
	textureCleanup();
	moduleTextureDelete();
}