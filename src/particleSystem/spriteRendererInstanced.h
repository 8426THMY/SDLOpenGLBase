#ifndef spriteRendererInstanced_h
#define spriteRendererInstanced_h


#include <stddef.h>

#include "sprite.h"

#include "utilTypes.h"

#include "settingsSprites.h"


// The shader used for instanced rendering has a uniform
// block large enough to store the data for each instance.
// It should go without saying that the maximum number of
// instances here should match the one in the shader!
#define SPRITE_RENDERER_INSTANCED_BUFFER_MAX_INSTANCES 40
#define SPRITE_RENDERER_INSTANCED_INSTANCE_BUFFER_SIZE \
	(SPRITE_RENDERER_INSTANCED_BUFFER_MAX_INSTANCES*sizeof(spriteInstancedData))


typedef struct spriteInstanced {
    unsigned int vertexArrayID;
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	unsigned int instanceBufferID;
	size_t numIndices;
} spriteInstanced;

typedef struct spriteInstancedData {
	mat4 state;
	rectangle uvOffsets;
} spriteInstancedData;

typedef struct spriteRendererInstanced {
	const spriteInstanced *base;
	spriteInstancedData *instances;
	size_t numInstances;
} spriteRendererInstanced;


void spriteRendererInstancedSetup();

void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedOffset(spriteRendererInstanced *const restrict instancedRenderer);
return_t spriteRendererInstancedHasRoom(
	const spriteRendererInstanced *const restrict instancedRenderer, const size_t numInstances
);
void spriteRendererInstancedDraw(const spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer);

void spriteRendererInstancedCleanup();


#endif