#ifndef moduleTextureGroup_h
#define moduleTextureGroup_h


#include "textureGroup.h"

#include "memoryPool.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_TEXGROUP
#define MODULE_TEXGROUP_SETUP_FAIL 3

#ifndef MEMORY_MODULE_NUM_TEXGROUPS
	#define MEMORY_MODULE_NUM_TEXGROUPS 1
#endif

#define MODULE_TEXGROUP_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_TEXGROUPS, sizeof(textureGroup))


// textureGroup
moduleDeclarePool(TexGroup, textureGroup, g_texGroupManager)
moduleDeclarePoolFree(TexGroup, textureGroup)
textureGroup *moduleTexGroupFind(const char *const restrict name);

return_t moduleTexGroupSetup();
void moduleTexGroupCleanup();


#endif