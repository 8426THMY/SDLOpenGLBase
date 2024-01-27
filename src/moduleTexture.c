#include "moduleTexture.h"


#include "memoryManager.h"


moduleDefinePool(Texture, texture, g_textureManager, textureDelete, MODULE_TEXTURE_MANAGER_SIZE)

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
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		memPoolInit(
			&g_textureManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXTURE_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_TEXTURE_MANAGER_SIZE), MODULE_TEXTURE_ELEMENT_SIZE
		) != NULL &&
		textureSetup()
	);
}

void moduleTextureCleanup(){
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		moduleTextureFree(i);
	MEMPOOL_LOOP_END(g_textureManager, i)
	textureCleanup();
	memoryManagerGlobalDeleteRegions(g_textureManager.region);
}