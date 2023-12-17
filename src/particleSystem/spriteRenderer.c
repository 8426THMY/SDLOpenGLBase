#include "particleSystemRenderer.h"


void spriteRendererInit(spriteRenderer *const restrict renderer, const spriteRendererType_t type){
	switch(type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedInit(&renderer->data.batchedRenderer);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedInit(&renderer->data.instancedRenderer);
		break;
	}
	renderer->type = type;
}

return_t spriteRendererHasRoom(const spriteRenderer *const restrict renderer, const size_t batchSize){
	switch(renderer->type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedHasRoom(&renderer->data.batchedRenderer, batchSize);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedHasRoom(&renderer->data.instancedRenderer, batchSize);
		break;
	}
}

void spriteRendererDraw(spriteRenderer *const restrict renderer){
	switch(renderer->type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedDraw(&renderer->data.batchedRenderer);
			spriteRendererBatchedOffset(&renderer->data.batchedRenderer);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedDraw(&renderer->data.instancedRenderer);
			spriteRendererInstancedOffset(&renderer->data.instancedRenderer);
		break;
	}
}

void spriteRendererDrawFull(spriteRenderer *const restrict renderer){
	switch(renderer->type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedDraw(&renderer->data.batchedRenderer);
			spriteRendererBatchedOrphan(&renderer->data.batchedRenderer);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedDraw(&renderer->data.instancedRenderer);
			spriteRendererInstancedOrphan(&renderer->data.instancedRenderer);
		break;
	}
}