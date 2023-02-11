#include "particleRenderer.h"


void (*const particleRendererInitBatchTable[PARTICLE_RENDERER_NUM_TYPES])(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
) = {

	particleRendererSpriteInitBatch,
	particleRendererBeamInitBatch,
	particleRendererMeshInitBatch
};

void (*const particleRendererBatchTable[PARTICLE_RENDERER_NUM_TYPES])(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
) = {

	particleRendererSpriteBatch,
	particleRendererBeamBatch,
	particleRendererMeshBatch
};


// Initialize a render batch using a particle renderer.
void particleRendererInitBatch(
	const particleRenderer *const restrict renderer,
	renderBatch *const restrict batch
){

	particleRendererInitBatchTable[renderer->type](
		(const void *const)&renderer->data, batch
	);
}

/*
** Fill a render batch with particles, ready to be drawn!
** If the buffer is completely filled during this process,
** it will be automatically drawn.
*/
void particleRendererBatch(
	const particleRenderer *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
){

	particleRendererBatchTable[renderer->type](
		(const void *const)&renderer->data, manager, batch, cam, dt
	);
}