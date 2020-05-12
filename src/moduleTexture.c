#include "moduleTexture.h"


memoryPool g_textureManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTextureSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		memPoolInit(
			&g_textureManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXTURE_MANAGER_SIZE)),
			MODULE_TEXTURE_MANAGER_SIZE, MODULE_TEXTURE_ELEMENT_SIZE
		) != NULL &&
		textureSetupDefault()
	);
}

void moduleTextureCleanup(){
	moduleTextureClear();
	memoryManagerGlobalFree(memPoolRegionStart(g_textureManager.region));
}


// Allocate memory for a texture and return a handle to it.
texture *moduleTextureAlloc(){
	return(memPoolAlloc(&g_textureManager));
}

// Free a texture that has been allocated.
void moduleTextureFree(texture *const restrict tex){
	textureDelete(tex);
	memPoolFree(&g_textureManager, tex);
}

// Delete every texture in the manager.
void moduleTextureClear(){
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		moduleTextureFree(i);
	MEMPOOL_LOOP_END(g_textureManager, i, return)
}


// Find a texture whose name matches "name"!
texture *moduleTextureFind(const char *const restrict name){
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_textureManager, i, return(NULL))

	return(NULL);
}