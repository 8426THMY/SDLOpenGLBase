#include "moduleTextureGroup.h"


memoryPool texGroupManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTexGroupSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager.
	return(
		memPoolInit(
			&texGroupManager,
			memoryManagerGlobalAlloc(memPoolMemoryForBlocks(MEMORY_MAX_TEXGROUPS, sizeof(textureGroup))),
			MEMORY_MAX_TEXGROUPS, sizeof(textureGroup)
		) != NULL
	);
}


//Allocate memory for a textureGroup
//and return a handle to it.
textureGroup *moduleTexGroupAlloc(){
	return(memPoolAlloc(&texGroupManager));
}

//Free a textureGroup that has been allocated.
void moduleTexGroupFree(textureGroup *texGroup){
	texGroupDelete(texGroup);
	memPoolFree(&texGroupManager, texGroup);
}


void moduleTexGroupCleanup(){
	/*size_t i = loadedTextureGroups.size;
	//Loop through the textureGroup manager.
	while(i > 0){
		--i;
		texGroupDelete((textureGroup *)vectorGet(&loadedTextureGroups, i));
	}*/
	memoryManagerGlobalFree(texGroupManager.region->start);
}


//Find a textureGroup whose name matches "name".
textureGroup *moduleTexGroupFind(const char *name){
	return(NULL);
}