#include "moduleTextureGroup.h"


#include "memoryManager.h"


// textureGroup
moduleDefinePool(TexGroup, textureGroup, g_texGroupManager, MODULE_TEXGROUP_MANAGER_SIZE)
moduleDefinePoolFreeFlexible(TexGroup, textureGroup, g_texGroupManager, texGroupDelete)

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
	return(moduleTexGroupInit());
}

void moduleTexGroupCleanup(){
	moduleTexGroupDelete();
}