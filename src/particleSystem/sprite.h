#ifndef sprite_h
#define sprite_h


#include <stdint.h>
#include <stddef.h>

#include "vec3.h"
#include "vec2.h"

#include "camera.h"

#include "utilTypes.h"

#include "settingsSprites.h"


// By default, we allow 100,000 quads to be batched at once.
#define SPRITE_BUFFER_MAX_QUADS    100000
#define SPRITE_BUFFER_MAX_VERTICES (SPRITE_BUFFER_MAX_QUADS*4)
#define SPRITE_VERTEX_BUFFER_SIZE  (SPRITE_BUFFER_MAX_VERTICES*sizeof(spriteVertex))
// Using triangle strips, we normally expect 4 indices per quad.
// However, we need to add an extra index for primitive restart,
// since we'll often be batching many quads together in one call.
#define SPRITE_BUFFER_MAX_INDICES (SPRITE_BUFFER_MAX_QUADS*5)
#define SPRITE_INDEX_BUFFER_SIZE  (SPRITE_BUFFER_MAX_INDICES*sizeof(spriteVertexIndex))

// By default, OpenGL uses a value of "-1" as
// its primitive restart index for triangle strips.
#define SPRITE_PRIMITIVE_RESTART_INDEX valueInvalid(unsigned int)


typedef struct spriteVertex {
	vec3 pos;
	vec2 uv;
} spriteVertex;

typedef uint_least32_t spriteVertexIndex;

// It's useful to have different shapes for our sprites.
// This could be used, for instance, to reduce overdraw.
typedef struct sprite {
	spriteVertex *vertices;
	size_t numVertices;
	spriteVertexIndex *indices;
	size_t numIndices;
} sprite;
// We use sprite buffers to store batches.
typedef sprite spriteBatch;


void spriteSetup();

void spriteBatchInit(spriteBatch *const restrict batch);
return_t spriteBatchHasRoom(
	const spriteBatch *const restrict batch,
	const sprite *const restrict spriteData,
	const size_t numSprites
);
void spriteDraw(const spriteBatch *const restrict batch);

void spriteCleanup();


#endif