#include "moduleTextureGroup.h"


#include "memoryManager.h"


moduleDefinePool(TexGroup, textureGroup, g_texGroupManager, texGroupDelete, MODULE_TEXGROUP_MANAGER_SIZE)

// Find a textureGroup whose name matches the one specified!
textureGroup *moduleTexGroupFind(const char *const restrict name){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup)
		if(strcmp(name, i->name) == 0){
			return(i);
		}
	MEMPOOL_LOOP_END(g_texGroupManager, i)

	return(&g_texGroupDefault);
}


return_t moduleTexGroupSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		memPoolInit(
			&g_texGroupManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_TEXGROUP_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_TEXGROUP_MANAGER_SIZE), MODULE_TEXGROUP_ELEMENT_SIZE
		) != NULL
	);
}

void moduleTexGroupCleanup(){
	MEMPOOL_LOOP_BEGIN(g_texGroupManager, i, textureGroup)
		moduleTexGroupFree(i);
	MEMPOOL_LOOP_END(g_texGroupManager, i)
	memoryManagerGlobalDeleteRegions(g_texGroupManager.region);
}