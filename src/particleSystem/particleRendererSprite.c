#include "particleRendererSprite.h"


#include "mat3x4.h"
#include "transform.h"

#include "particleManager.h"


void particleRendererSpriteInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
){

	// Draw the old batch if it isn't compatible with the new one!
	#warning "We need to check the textures in use here."
	#warning "Renderers should probably have a shared component that lets us check compatibility easily."
	if(batch->type != SPRITE_RENDERER_TYPE_BATCHED){
		#error "This is all probably incorrect. Same for the other renderers."
		spriteRendererDraw(batch);
		spriteRendererInit(batch, SPRITE_RENDERER_TYPE_BATCHED);

		#warning "Bind any textures or uniforms here!"
	}
}

/*
** Return the number of indices we'll need for the batch,
** as we always have at least as many indices as vertices.
** We also add one to account for primitive restart.
*/
size_t particleRendererSpriteBatchSize(
	const void *const restrict renderer,
	const particleManager *const restrict manager
){

	return(
		manager->numParticles * (
			((const particleRendererSprite *const)renderer)->spriteData.numIndices + 1
		)
	);
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
*/
void particleRendererSpriteBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
){

	// Exit early if the manager has no particles.
	if(manager->numParticles > 0){
		const particleRendererSprite partRenderer =
			*((const particleRendererSprite *const)renderer);
		const spriteVertex *const lastBaseVertex     = &baseVertex[partRenderer.spriteData.numVertices];
		const spriteVertexIndex *const lastBaseIndex = &baseIndex[partRenderer.spriteData.numIndices];

		const particle *curParticle = manager->particles;
		const particle *const lastParticle = &curParticle[manager->numParticles];
		spriteRendererBatched *const batchedRenderer = &batch->data.batchedRenderer;

		for(; curParticle != lastParticle; ++curParticle){
			const spriteVertex *baseVertex     = partRenderer.spriteData.vertices;
			const spriteVertexIndex *baseIndex = partRenderer.spriteData.indices;
			transform curTransform;
			mat3x4 curState;
			size_t startIndex;

			// If the batch is full, draw it and start a new one!
			// We add an extra index to account for primitive restart.
			if(spriteRendererBatchedHasRoom(
				batchedRenderer
				partRenderer.spriteData.numVertices, partRenderer.spriteData.numIndices + 1
			)){

				spriteRendererBatchedDraw(batchedRenderer);
				spriteRendererBatchedOrphan(batchedRenderer);
			}

			// Compute the interpolated particle transform.
			transformInterpSet(
				&curParticle->subsys.state[1],
				&curParticle->subsys.state[0],
				dt, &curTransform
			);
			// Note: It is almost always faster to convert transforms to matrices
			// and then transform our points, rather than transforming them directly.
			// The only exception is if we're only transforming a single point, but
			// even this is only negligibly slower.
			transformToMat3x4(&curTransform, &curState);
			// We only store relative indices, so we need to
			// add the correct index to start counting from.
			startIndex = batchedRenderer->numVertices;

			// Add this instance's vertices to the buffer.
			for(; baseVertex != lastBaseVertex; ++baseVertex){
				spriteVertex curVertex;
				#warning "Temporary, until we know how we want to do billboarding."
				mat3x4MultiplyVec3Out(&curState, &baseVertex->pos, &curVertex.pos);
				#warning "Temporary, until we know how we want to do textures."
				curVertex.uv = baseVertex->uv;
				curVertex.normal = baseVertex->normal;

				// Add the vertex to the batch.
				spriteRendererBatchedAddVertex(batchedRenderer, curVertex);
			}
			// Add this instance's indices to the buffer.
			for(; baseIndex != lastBaseIndex; ++baseIndex){
				if(*baseIndex == SPRITE_PRIMITIVE_RESTART_INDEX){
					spriteRendererBatchedAddIndex(batchedRenderer, SPRITE_PRIMITIVE_RESTART_INDEX);
				}else{
					spriteRendererBatchedAddIndex(batchedRenderer, startIndex + *baseIndex);
				}
			}
			// Add an index for primitive restart, since we're
			// using triangle strips. This means we won't interfere
			// with the next batch if it also fits in the buffer.
			spriteRendererBatchedAddIndex(batchedRenderer, SPRITE_PRIMITIVE_RESTART_INDEX);
		}
	}
}