#include "particleRendererPoint.h"


#include "particleManager.h"


/*
** Return the number of indices we'll need for the batch.
** For point sprites, this is just the number of particles.
** We don't need to worry about primitive restart or anything.
*/
size_t particleRendererPointBatchSize(
	const particleRendererPoint *const restrict renderer,
	const size_t numParticles
){

	return(numParticles);
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
*/
void particleRendererPointBatch(
	const particleRendererPoint *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
){

	/*// Exit early if the manager has no particles.
	if(numParticles > 0){
		const particleRendererPoint pointRenderer =
			*((const particleRendererPoint *const)renderer);
	}*/
}