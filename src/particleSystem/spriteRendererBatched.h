#ifndef spriteRendererBatched_h
#define spriteRendererBatched_h


#include <stddef.h>

#include "sprite.h"

#include "utilTypes.h"

#include "settingsSprites.h"


// By default, we allow 25,000 quads to be batched at once.
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS    25000
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_VERTICES \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS*4)
#define SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE  \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_VERTICES*sizeof(spriteVertex))
// Using triangle strips, we normally expect 4 indices per quad.
// However, we need to add an extra index for primitive restart,
// since we'll often be batching many quads together in one call.
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_INDICES \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS*5)
#define SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE  \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_INDICES*sizeof(spriteVertexIndex))

#define SPRITE_RENDERER_BATCHED_BUFFER_SIZE \
	SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE + SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE


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
	spriteVertex *curVertex;
	spriteVertexIndex *curIndex;
	size_t numVertices;
	size_t numIndices;
} spriteRendererBatched;


void spriteRendererBatchedSetup();

void spriteRendererBatchedInit(spriteRendererBatched *const restrict batchedRenderer);
void spriteRendererBatchedOffset(spriteRendererBatched *const restrict batchedRenderer);
return_t spriteRendererBatchedHasRoom(
	const spriteRendererBatched *const restrict batchedRenderer, const size_t numIndices
);
void spriteRendererBatchedDraw(const spriteRendererBatched *const restrict batchedRenderer);
void spriteRendererBatchedOrphan(spriteRendererBatched *const restrict batchedRenderer);

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