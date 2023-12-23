#ifndef spriteRendererInstanced_h
#define spriteRendererInstanced_h


#include <stddef.h>

#include "sprite.h"

#include "utilTypes.h"


typedef struct spriteInstanced {
	unsigned int vertexArrayID;
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	size_t numIndices;
} spriteInstanced;

typedef struct spriteInstancedData {
	mat3x4 state;
	rectangle uvOffsets;
} spriteInstancedData;

typedef struct spriteRendererInstanced {
	#warning "Should this be stored elsewhere?"
	const spriteInstanced *base;
	// OpenGL handles allocating and freeing the array
	// of instances, so we only need to store a pointer
	// to the one we're currently operating on.
	//
	// It's worth mentioning that we are only allowed to
	// write to this location. Reading it is illegal!
	spriteInstancedData *curInstance;
	size_t numInstances;
} spriteRendererInstanced;


void spriteRendererInstancedSetup();

void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedDraw(const spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer);

void spriteInstancedGenerateBuffers(
	spriteInstanced *const restrict spriteData,
	const spriteVertex *const restrict vertices, const spriteVertexIndex numVertices,
	const spriteVertexIndex *const restrict indices, const spriteVertexIndex numIndices
);
return_t spriteRendererInstancedIsFull(const spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedAddInstance(spriteRendererInstanced *const restrict instancedRenderer);

void spriteRendererInstancedCleanup();


#endif