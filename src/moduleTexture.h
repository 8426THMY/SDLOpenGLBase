#ifndef moduleTexture_h
#define moduleTexture_h


#include "texture.h"

#include "memoryPool.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_TEXTURE
#define MODULE_TEXTURE_SETUP_FAIL 2

#ifndef MEMORY_MODULE_NUM_TEXTURES
	#define MEMORY_MODULE_NUM_TEXTURES 1
#endif

#define MODULE_TEXTURE_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_TEXTURES, sizeof(texture))


// texture
moduleDeclarePool(Texture, texture, g_textureManager)
moduleDeclarePoolFree(Texture, texture)
texture *moduleTextureFind(const char *const restrict name);

return_t moduleTextureSetup();
void moduleTextureCleanup();


#endif