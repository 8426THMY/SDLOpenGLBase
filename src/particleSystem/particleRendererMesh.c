#include "particleRendererSprite.h"


#include "mat3x4.h"
#include "transform.h"

#include "particleManager.h"


void particleRendererMeshInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
){

	// Draw the old batch if it isn't compatible with the new one!
	#warning "We need to check the textures in use here."
	#warning "Renderers should probably have a shared component that lets us check compatibility easily."
	if(batch->type != SPRITE_RENDERER_TYPE_INSTANCED){
		#error "This is all probably incorrect. Same for the other renderers."
		spriteRendererDraw(batch);
		spriteRendererInit(batch, SPRITE_RENDERER_TYPE_INSTANCED);

		#warning "Bind any textures or uniforms here!"
	}
}

// The batch size is just the number of instances.
size_t particleRendererMeshBatchSize(
	const void *const restrict renderer,
	const particleManager *const restrict manager
){

	return(manager->numParticles);
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
*/
void particleRendererMeshBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
){

	// Exit early if the manager has no particles.
	if(manager->numParticles > 0){
		const particleRendererMesh partRenderer =
			*((const particleRendererMesh *const)renderer);

		const particle *curParticle = manager->particles;
		const particle *const lastParticle = &curParticle[manager->numParticles];
		spriteRendererInstanced *const instancedRenderer = &batch->data.instancedRenderer;

		for(; curParticle != lastParticle; ++curParticle){
			transform curTransform;
			spriteInstancedData curInstance;

			// If the batch is full, draw it and start a new one!
			if(spriteRendererInstancedIsFull(instancedRenderer)){
				spriteRendererInstancedDraw(instancedRenderer);
				spriteRendererInstancedOrphan(instancedRenderer);
			}

			// Compute the interpolated particle transform.
			transformInterpSet(
				&curParticle->subsys.state[1],
				&curParticle->subsys.state[0],
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