#include "particleRenderer.h"


spriteRendererType particleRendererTypeTable[PARTICLE_RENDERER_NUM_TYPES] = {
	SPRITE_RENDERER_TYPE_BATCHED,
	SPRITE_RENDERER_TYPE_BATCHED,
	SPRITE_RENDERER_TYPE_BATCHED,
	SPRITE_RENDERER_TYPE_INSTANCED
};


// Initialize a render batch using a particle renderer.
void particleRendererInitBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch
){

	const spriteRendererType type = particleRendererTypeTable[renderer->type];
	// Draw the old batch if it isn't compatible with the new one!
	if(!spriteRendererCompatible(batch, &renderer->common, type)){
		spriteRendererDraw(batch);
		spriteRendererInit(batch, &renderer->common, type);

		#warning "Bind any textures or uniforms here!"
	}
}

// Return the size of the batch for this renderer and manager pair.
size_t particleRendererBatchSize(
	const particleRenderer *const restrict renderer, const size_t numParticles
){

	switch(renderer->type){
		case PARTICLE_RENDERER_POINT:
			return(particleRendererPointBatchSize(
				&renderer->data.pointRenderer, numParticles
			));
		break;
		case PARTICLE_RENDERER_SPRITE:
			return(particleRendererSpriteBatchSize(
				&renderer->data.spriteRenderer, numParticles
			));
		break;
		case PARTICLE_RENDERER_BEAM:
			return(particleRendererPointBatchSize(
				&renderer->data.beamRenderer, numParticles
			));
		break;
		case PARTICLE_RENDERER_MESH:
			return(particleRendererPointBatchSize(
				&renderer->data.meshRenderer, numParticles
			));
		break;
	}
	return(0);
}

/*
** Fill a render batch with particles, ready to be drawn!
** If the buffer is completely filled during this process,
** it will be automatically drawn.
*/
void particleRendererBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
){

	switch(renderer->type){
		case PARTICLE_RENDERER_POINT:
			particleRendererPointBatch(
				&renderer->data.pointRenderer, batch,
				keyValues, numParticles, cam, dt
			);
		break;
		case PARTICLE_RENDERER_SPRITE:
			particleRendererSpriteBatch(
				&renderer->data.spriteRenderer, batch,
				keyValues, numParticles, cam, dt
			);
		break;
		case PARTICLE_RENDERER_BEAM:
			particleRendererPointBatch(
				&renderer->data.beamRenderer, batch,
				keyValues, numParticles, cam, dt
			);
		break;
		case PARTICLE_RENDERER_MESH:
			particleRendererPointBatch(
				&renderer->data.meshRenderer, batch,
				keyValues, numParticles, cam, dt
			);
		break;
	}
}