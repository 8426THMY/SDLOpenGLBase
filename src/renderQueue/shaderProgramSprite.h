#ifndef shaderProgramSprite_h
#define shaderProgramSprite_h


#include "vec4.h"

#include "utilTypes.h"


typedef struct shaderProgramSprite {
	unsigned int spriteDataID;
} shaderProgramSprite;


void shaderPrgSpriteInit(
	shaderProgramSprite *const restrict shader,
	const unsigned int shaderID
);

void shaderPrgSpriteSetup();
void shaderPrgSpriteLoadSharedUniforms(
	const unsigned int sdfType,
	const vec4 *const restrict sdfColour,
	const vec4 *const restrict sdfBackground
);
void shaderPrgSpriteCleanup();


#endif