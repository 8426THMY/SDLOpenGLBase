#include "moduleTextureGroup.h"


memoryPool g_texGroupManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTexGroupSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		memPoolInit(
			&g_texGroupManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXGROUP_MANAGER_SIZE)),
			MODULE_TEXGROUP_MANAGER_SIZE, MODULE_TEXGROUP_ELEMENT_SIZE
		) != NULL
	);
}

void moduleTexGroupCleanup(){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup)
		moduleTexGroupFree(i);
	MEMPOOL_LOOP_END(g_texGroupManager, i)
	memPoolDelete(&g_texGroupManager, memoryManagerGlobalFree);
}


// Allocate memory for a textureGroup and return a handle to it.
textureGroup *moduleTexGroupAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_texGroupManager));
	#else
	textureGroup *newBlock = memPoolAlloc(&g_texGroupManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_texGroupManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXGROUP_MANAGER_SIZE)),
			MODULE_TEXGROUP_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_texGroupManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a textureGroup that has been allocated.
void moduleTexGroupFree(textureGroup *const restrict texGroup){
	texGroupDelete(texGroup);
	memPoolFree(&g_texGroupManager, texGroup);
}

// Delete every texture group in the manager.
void moduleTexGroupClear(){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup)
		moduleTexGroupFree(i);
	MEMPOOL_LOOP_END(g_texGroupManager, i)
	memPoolClear(&g_texGroupManager);
}


// Find a textureGroup whose name matches the one specified!
textureGroup *moduleTexGroupFind(const char *const restrict name){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_texGroupManager, i)

	return(&g_texGroupDefault);
}