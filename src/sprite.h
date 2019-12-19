#ifndef sprite_h
#define sprite_h


#include "mesh.h"

#include "utilTypes.h"


typedef struct sprite {
	mesh *meshData;
} sprite;


return_t spriteSetupDefault();


extern mesh spriteMeshDefault;


#endif