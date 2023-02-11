#include "particleRendererSprite.h"


#include "mat4.h"
#include "transform.h"

#include "particleManager.h"


/*
** Fill the instance buffer up with particle data from
** each particle system node in the array specified.
*/
void particleRendererMeshInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
){

	if(batch->type != RENDER_BATCH_TYPE_MESH){
		// Draw the previous batch if it's still in use.
		if(batch->type != RENDER_BATCH_TYPE_UNUSED){
			renderBatchDraw(batch);
		}
		meshBatchInit(&batch.mBatch);
		batch->type = RENDER_BATCH_TYPE_MESH;

		#warning "Bind any textures or uniforms here!"
		#warning "We also need some way of checking what is already bound."
	}
}

void particleRendererMeshBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
){

	// Exit early if the manager has no particles.
	if(manager->numParticles > 0){
		const particleRendererMesh *const meshRenderer =
			(const particleRendererMesh *const)renderer;
		const mesh *const meshData = meshRenderer->meshData;

		const particle *curParticle = manager->first;
		meshBatch *const mBatch = (meshBatch *const)&batch->mBatch;
		meshInstance *curInstance = &mBatch->instances[mBatch->numInstances];

		for(; curParticle != NULL; curParticle = curParticle->next){
			// If the batch is full, draw it and start a new one!
			if(mBatch->numInstances == MESH_BUFFER_MAX_INSTANCES){
				meshBatchDraw(mBatch);
				meshBatchInit(mBatch);
			}

			#warning "Temporary, until we know how we want to do billboarding."
			transformToMat4(&curParticle->state[0], &curInstance->state);
			#warning "Temporary, until we know how we want to do textures."
			curInstance->uvOffsets.x = 0.f;
			curInstance->uvOffsets.y = 0.f;
			curInstance->uvOffsets.w = 1.f;
			curInstance->uvOffsets.h = 1.f;

			++curInstance;
		}
	}
}