#ifndef moduleTexture_h
#define moduleTexture_h


#define MODULETEXTURE

#define MODULETEXTURE_SETUP_FAIL 1


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"

#include "texture.h"


return_t moduleTextureSetup();

texture *moduleTextureAlloc();
void moduleTextureFree(texture *tex);

void moduleTextureCleanup();


texture *moduleTextureFind(const char *name);


extern memoryPool textureManager;


#endif