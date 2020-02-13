#ifndef moduleTextureGroup_h
#define moduleTextureGroup_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "textureGroup.h"


#define MODULE_TEXGROUP
#define MODULE_TEXGROUP_SETUP_FAIL 2

#define MODULE_TEXGROUP_ELEMENT_SIZE sizeof(textureGroup)

#ifndef MEMORY_MODULE_NUM_TEXGROUPS
	#define MEMORY_MODULE_NUM_TEXGROUPS 1
#endif

#define MODULE_TEXGROUP_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_TEXGROUPS, MODULE_TEXGROUP_ELEMENT_SIZE)


return_t moduleTexGroupSetup();
void moduleTexGroupCleanup();

textureGroup *moduleTexGroupAlloc();
void moduleTexGroupFree(textureGroup *const restrict texGroup);
void moduleTexGroupClear();

textureGroup *moduleTexGroupFind(const char *const restrict name);


extern memoryPool g_texGroupManager;


#endif