#ifndef spriteRendererBatched_h
#define spriteRendererBatched_h


#include <stddef.h>

#include "sprite.h"

#include "utilTypes.h"


/*
** We use something similar to the approach proposed by Rob Barris:
**     https://community.khronos.org/t/vbos-strangely-slow/60109/32
** It's possible to improve performance in our solution by using
** persistent buffers, which mean we don't need to keep mapping
** and unmapping. Of course, this is an OpenGL 4.4+ feature, so
** it isn't possible if we want to keep sticking to OpenGL 3.3.
*/
typedef struct spriteRendererBatched {
	// OpenGL handles allocating and freeing the arrays
	// of vertices and indices, so we only need to store
	// pointers to the ones we're currently operating on.
	//
	// It's worth mentioning that we are only allowed to
	// write to these locations. Reading them is illegal!
	spriteVertex *curVertex;
	spriteVertexIndex *curIndex;
	size_t numVertices;
	size_t numIndices;
} spriteRendererBatched;


void spriteRendererBatchedSetup();

void spriteRendererBatchedInit(spriteRendererBatched *const restrict batchedRenderer);
void spriteRendererBatchedDraw(const spriteRendererBatched *const restrict batchedRenderer);
void spriteRendererBatchedOrphan(spriteRendererBatched *const restrict batchedRenderer);

return_t spriteRendererBatchedHasRoom(
	const spriteRendererBatched *const restrict batchedRenderer,
	const size_t numVertices, const size_t numIndices
);
void spriteRendererBatchedAddVertex(
	spriteRendererBatched *const restrict batchedRenderer,
	const spriteVertex *const restrict vertex
);
void spriteRendererBatchedAddIndex(
	spriteRendererBatched *const restrict batchedRenderer,
	const spriteVertexIndex index
);

void spriteRendererBatchedCleanup();


#endif