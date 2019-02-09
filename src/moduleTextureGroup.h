#ifndef moduleTextureGroup_h
#define moduleTextureGroup_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "textureGroup.h"


#define MODULETEXGROUP
#define MODULETEXGROUP_SETUP_FAIL 2

#ifndef MEMORY_MAX_TEXGROUPS
	#define MEMORY_MAX_TEXGROUPS 1
#endif


return_t moduleTexGroupSetup();

textureGroup *moduleTexGroupAlloc();
void moduleTexGroupFree(textureGroup *texGroup);

void moduleTexGroupCleanup();


textureGroup *moduleTexGroupFind(const char *name);


extern memoryPool texGroupManager;


#endif