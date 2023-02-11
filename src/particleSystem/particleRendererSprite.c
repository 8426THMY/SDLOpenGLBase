#include "particleRendererSprite.h"


#include "mat4.h"
#include "transform.h"

#include "particleManager.h"


void particleRendererSpriteInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
){

	if(batch->type != RENDER_BATCH_TYPE_SPRITE){
		// Draw the previous batch if it's still in use.
		if(batch->type != RENDER_BATCH_TYPE_UNUSED){
			renderBatchDraw(batch);
		}
		spriteBatchInit(&batch.sBatch);
		batch->type = RENDER_BATCH_TYPE_SPRITE;

		#warning "Bind any textures or uniforms here!"
		#warning "We also need some way of checking what is already bound."
	}
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
*/
void particleRendererSpriteBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
){

	// Exit early if the manager has no particles.
	if(manager->numParticles > 0){
		const particleRendererSprite *const spriteRenderer =
			(const particleRendererSprite *const)renderer;
		const sprite *const spriteData = spriteRenderer->spriteData;
		const spriteVertex *const lastBaseVertex = &baseVertex[spriteData->numVertices];
		const spriteIndex *const lastBaseIndex   = &baseIndex[spriteData->numIndices];

		const particle *curParticle = manager->first;
		spriteBatch *const sBatch = (spriteBatch *const)&batch->sBatch;
		spriteVertex *curVertex = &sBatch->vertices[sBatch->numVertices];
		spriteIndex *curIndex   = &sBatch->indices[sBatch->numIndices];

		for(; curParticle != NULL; curParticle = curParticle->next){
			const spriteVertex *baseVertex = spriteData->vertices;
			const spriteIndex *baseIndex   = spriteData->indices;
			mat4 curState;
			size_t startIndex;

			// If the batch is full, draw it and start a new one!
			if(!spriteBatchHasRoom(sBatch, spriteData, 1)){
				spriteBatchDraw(sBatch);
				spriteBatchInit(sBatch);
				// Reset the vertex and index pointers.
				curVertex = sBatch->vertices;
				curIndex  = sBatch->indices;
			}

			// Note: It is almost always faster to convert transforms to matrices
			//       and then transform our points, rather than transforming them
			//       directly. The only exception is if we're only transforming a
			//       single point, but even this is only negligibly slower.
			transformToMat4(&curParticle->state[0], &curState);
			// We only store relative indices, so we need to
			// add the correct index to start counting from.
			startIndex = sBatch->numVertices;

			// Add this instance's vertices to the buffer.
			for(; baseVertex != lastVertex; ++baseVertex){
				#warning "Temporary, until we know how we want to do billboarding."
				mat4MultiplyVec3Out(&curState, &baseVertex->pos, &curVertex->pos);
				#warning "Temporary, until we know how we want to do textures."
				curVertex->uv = baseVertex->uv;
				++curVertex;
			}
			// Add this instance's indices to the buffer.
			for(; baseIndex != lastIndex; ++baseIndex){
				if(*baseIndex == SPRITE_PRIMITIVE_RESTART_INDEX){
					*curIndex = SPRITE_PRIMITIVE_RESTART_INDEX;
				}else{
					*curIndex = startIndex + *baseIndex;
				}
				++curIndex;
			}

			// Add an index for primitive restart,
			// since we're using triangle strips.
			*curIndex = SPRITE_PRIMITIVE_RESTART_INDEX;
			++curIndex;

			// Update the vertex and index counts. Note that we
			// add one to the index count due to primitive restart.
			sBatch->numVertices += spriteData->numVertices;
			sBatch->numIndices  += spriteData->numIndices + 1;
		}
	}
}