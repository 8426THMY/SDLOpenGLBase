#ifndef moduleTexture_h
#define moduleTexture_h


#include "texture.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_TEXTURE
#define MODULE_TEXTURE_SETUP_FAIL 2

#define MODULE_TEXTURE_ELEMENT_SIZE sizeof(texture)

#ifndef MEMORY_MODULE_NUM_TEXTURES
	#define MEMORY_MODULE_NUM_TEXTURES 1
#endif

#define MODULE_TEXTURE_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_TEXTURES, MODULE_TEXTURE_ELEMENT_SIZE)


moduleDeclarePool(Texture, texture, g_textureManager)
texture *moduleTextureFind(const char *const restrict name);

return_t moduleTextureSetup();
void moduleTextureCleanup();


#endif