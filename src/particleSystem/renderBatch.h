#ifndef renderBatch_h
#define renderBatch_h


#include <stdint.h>

#include "sprite.h"
#include "mesh.h"


#define RENDER_BATCH_TYPE_UNUSED 0
#define RENDER_BATCH_TYPE_SPRITE 1
#define RENDER_BATCH_TYPE_MESH   2


typedef uint_least8_t renderBatchType_t;

typedef struct renderBatch {
	union {
		spriteBatch sBatch;
		meshBatch mBatch;
	} data;
	renderBatchType_t type;
} renderBatch;


void renderBatchInit(renderBatch *const restrict batch, const renderBatchType_t type);
void renderBatchDraw(const renderBatch *const restrict batch);


#endif