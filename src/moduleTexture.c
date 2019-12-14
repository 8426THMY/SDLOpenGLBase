#include "moduleTexture.h"


memoryPool textureManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTextureSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		memPoolInit(
			&textureManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXTURE_MANAGER_SIZE)),
			MODULE_TEXTURE_MANAGER_SIZE, MODULE_TEXTURE_ELEMENT_SIZE
		) != NULL &&
		textureSetupDefault()
	);
}

void moduleTextureCleanup(){
	moduleTextureClear();
	memoryManagerGlobalFree(memPoolRegionStart(textureManager.region));
}


// Allocate memory for a texture and return a handle to it.
texture *moduleTextureAlloc(){
	return(memPoolAlloc(&textureManager));
}

// Free a texture that has been allocated.
void moduleTextureFree(texture *tex){
	textureDelete(tex);
	memPoolFree(&textureManager, tex);
}

// Delete every texture in the manager.
void moduleTextureClear(){
	MEMPOOL_LOOP_BEGIN(textureManager, i, texture *)
		moduleTextureFree(i);
	MEMPOOL_LOOP_END(textureManager, i, texture *, return)
}


// Find a texture whose name matches "name"!
texture *moduleTextureFind(const char *name){
	MEMPOOL_LOOP_BEGIN(textureManager, i, texture *)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(textureManager, i, texture *, return(NULL))

	return(NULL);
}