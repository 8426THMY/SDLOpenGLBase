#ifndef sprite_h
#define sprite_h


#include "model.h"

#include "utilTypes.h"


typedef struct sprite {
	model *mdl;
} sprite;


return_t spriteSetupDefault();


extern model spriteMdlDefault;
extern sprite spriteDefault;


#endif