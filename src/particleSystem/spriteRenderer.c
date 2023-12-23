#include "spriteRenderer.h"


void spriteRendererInit(spriteRenderer *const restrict renderer, const spriteRendererType type){
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

void spriteRendererDraw(spriteRenderer *const restrict renderer){
	switch(renderer->type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedDraw(&renderer->data.batchedRenderer);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedDraw(&renderer->data.instancedRenderer);
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