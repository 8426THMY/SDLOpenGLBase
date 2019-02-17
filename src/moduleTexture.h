#ifndef moduleTexture_h
#define moduleTexture_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "texture.h"


#define MODULE_TEXTURE
#define MODULE_TEXTURE_SETUP_FAIL 1

#define MODULE_TEXTURE_ELEMENT_SIZE sizeof(texture)

#ifndef MEMORY_MODULE_NUM_TEXTURES
	#define MEMORY_MODULE_NUM_TEXTURES 1
#endif

#define MODULE_TEXTURE_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_TEXTURES, MODULE_TEXTURE_ELEMENT_SIZE)


return_t moduleTextureSetup();
void moduleTextureCleanup();

texture *moduleTextureAlloc();
void moduleTextureFree(texture *tex);
void moduleTextureClear();

texture *moduleTextureFind(const char *name);


extern memoryPool textureManager;


#endif