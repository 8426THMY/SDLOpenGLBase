#include "renderBatch.h"


void renderBatchInit(renderBatch *const restrict batch, const renderBatchType_t type){
	switch(batch->type){
		case RENDER_BATCH_TYPE_SPRITE:
			spriteBatchInit(&batch->data.sBatch);
		break;
		case RENDER_BATCH_TYPE_MESH:
			meshBatchInit(&batch->data.mBatch);
		break;
	}
	batch->type = type;
}

void renderBatchDraw(const renderBatch *const restrict batch){
	switch(batch->type){
		case RENDER_BATCH_TYPE_SPRITE:
			spriteBatchDraw(&batch->data.sBatch);
		break;
		case RENDER_BATCH_TYPE_MESH:
			meshBatchDraw(&batch->data.mBatch);
		break;
	}
}