#ifndef moduleTexture_h
#define moduleTexture_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "texture.h"


#define MODULETEXTURE
#define MODULETEXTURE_SETUP_FAIL 1

#ifndef MEMORY_MAX_TEXTURES
	#define MEMORY_MAX_TEXTURES 1
#endif


return_t moduleTextureSetup();

texture *moduleTextureAlloc();
void moduleTextureFree(texture *tex);

void moduleTextureCleanup();


texture *moduleTextureFind(const char *name);


extern memoryPool textureManager;


#endif