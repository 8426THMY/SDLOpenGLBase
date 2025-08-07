#ifndef spriteRendererInstanced_h
#define spriteRendererInstanced_h


#include <stddef.h>

#include "mesh.h"

#include "utilTypes.h"


typedef struct spriteRendererInstanced {
	#warning "Should this be stored elsewhere?"
	const mesh *base;
	// OpenGL handles allocating and freeing the array
	// of instances, so we only need to store a pointer
	// to the one we're currently operating on.
	//
	// It's worth mentioning that we are only allowed to
	// write to this location. Reading it is illegal!
	meshInstance *curInstance;
	size_t numInstances;
} spriteRendererInstanced;


void spriteRendererInstancedSetup();

void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedDraw(const spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer);

return_t spriteRendererInstancedIsFull(const spriteRendererInstanced *const restrict instancedRenderer);
void spriteRendererInstancedAddInstance(spriteRendererInstanced *const restrict instancedRenderer);

void spriteRendererInstancedCleanup();


#endif