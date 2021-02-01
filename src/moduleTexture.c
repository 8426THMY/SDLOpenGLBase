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
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		moduleTextureFree(i);
	MEMPOOL_LOOP_END(g_textureManager, i)
	memPoolDelete(&g_textureManager, memoryManagerGlobalFree);
}


// Allocate memory for a texture and return a handle to it.
texture *moduleTextureAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_textureManager));
	#else
	texture *newBlock = memPoolAlloc(&g_textureManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_textureManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXTURE_MANAGER_SIZE)),
			MODULE_TEXTURE_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_textureManager);
		}
	}
	return(newBlock);
	#endif
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
	MEMPOOL_LOOP_END(g_textureManager, i)
	memPoolClear(&g_textureManager);
}


// Find a texture whose name matches the one specified!
texture *moduleTextureFind(const char *const restrict name){
	MEMPOOL_LOOP_BEGIN(g_textureManager, i, texture)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_textureManager, i)

	return(&g_texDefault);
}