#include "spriteRenderer.h"


void spriteRendererInit(
	spriteRenderer *const restrict renderer,
	const spriteRendererCommon *const restrict common,
	const spriteRendererType type
){

	switch(type){
		case SPRITE_RENDERER_TYPE_BATCHED:
			spriteRendererBatchedInit(&renderer->data.batchedRenderer);
		break;
		case SPRITE_RENDERER_TYPE_INSTANCED:
			spriteRendererInstancedInit(&renderer->data.instancedRenderer);
		break;
	}
	renderer->common = *common;
	renderer->type = type;
}

return_t spriteRendererCompatible(
	const spriteRenderer *const restrict renderer,
	const spriteRendererCommon *const restrict common,
	const spriteRendererType type
){

	return(
		// Even when we haven't initialized the spriteRenderer,
		// the type is always set to SPRITE_RENDERER_TYPE_UNUSED.
		renderer->type == type &&
		#if 0
		renderer->common.texGroup == common->texGroup &&
		#endif
		renderer->common.blendSrc == common->blendSrc &&
		renderer->common.blendDest == common->blendDest &&
		renderer->common.drawMode == common->drawMode
	);
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