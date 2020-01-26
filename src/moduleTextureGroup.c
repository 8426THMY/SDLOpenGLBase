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
	moduleTexGroupClear();
	memoryManagerGlobalFree(memPoolRegionStart(g_texGroupManager.region));
}


// Allocate memory for a textureGroup and return a handle to it.
textureGroup *moduleTexGroupAlloc(){
	return(memPoolAlloc(&g_texGroupManager));
}

// Free a textureGroup that has been allocated.
void moduleTexGroupFree(textureGroup *texGroup){
	texGroupDelete(texGroup);
	memPoolFree(&g_texGroupManager, texGroup);
}

// Delete every texture group in the manager.
void moduleTexGroupClear(){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup *)
		moduleTexGroupFree(i);
	MEMPOOL_LOOP_END(g_texGroupManager, i, textureGroup *, return)
}


// Find a textureGroup whose name matches "name"!
textureGroup *moduleTexGroupFind(const char *name){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup *)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_texGroupManager, i, textureGroup *, return(NULL))

	return(NULL);
}