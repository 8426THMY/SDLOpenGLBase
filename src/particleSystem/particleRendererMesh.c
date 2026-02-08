#include "particleRendererSprite.h"


#include "mat3x4.h"
#include "transform.h"

#include "particleManager.h"


// The batch size is just the number of instances.
size_t particleRendererMeshBatchSize(
	const particleRendererMesh *const restrict renderer,
	const size_t numParticles
){

	return(numParticles);
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
*/
void particleRendererMeshBatch(
	const particleRendererMesh *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
){

	// Exit early if the manager has no particles.
	if(numParticles > 0){
		const keyValue *curKeyValue = keyValues;
		const keyValue *const lastKeyValue = &keyValues[numParticles];
		spriteRendererInstanced *const instancedRenderer = &batch->data.instancedRenderer;

		for(; curKeyValue != lastKeyValue; ++curKeyValue){
			const particle *const curParticle = curKeyValue->value;
			transform curTransform;
			spriteInstancedData curInstance;

			// If the batch is full, draw it and start a new one!
			if(spriteRendererInstancedIsFull(instancedRenderer)){
				spriteRendererInstancedDraw(instancedRenderer);
				spriteRendererInstancedOrphan(instancedRenderer);
			}

			// Compute the interpolated particle transform.
			transformInterpSet(
				&curParticle->prevState,
				&curParticle->state,
				dt, &curTransform
			);
			#warning "Temporary, until we know how we want to do billboarding."
			transformToMat3x4(&curTransform, &curInstance.state);
			#warning "Temporary, until we know how we want to do textures."
			curInstance.uvOffsets.x = 0.f;
			curInstance.uvOffsets.y = 0.f;
			curInstance.uvOffsets.w = 1.f;
			curInstance.uvOffsets.h = 1.f;

			// Add the instance to the batch!
			spriteRendererInstancedAddInstance(instancedRenderer, curInstance);
		}
	}
}