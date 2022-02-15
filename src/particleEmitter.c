#include "particleEmitter.h"


void particleEmitterInit(particleEmitter *const restrict emitter){
	emitter->elapsedTime = 0.f;
	emitter->period = 0.032f;
}


// Return the number of particles the emitter is allowed to spawn.
size_t particleEmitterUpdate(particleEmitter *const restrict emitter, const particleEmitterDef *const restrict emitterDef, const float dt){
	emitter->elapsedTime += dt;

	return((*emitterDef->func)(emitter));
}


size_t particleEmitterContinuous(particleEmitter *const restrict emitter){
	if(emitter->elapsedTime >= emitter->period){
		// Determine the number of particles to emit.
		const size_t spawnCount = emitter->elapsedTime / emitter->period;
		// Reduce the emitter's timer so we don't
		// spawn these particles again next time.
		emitter->elapsedTime -= ((float)spawnCount) * emitter->period;

		return(spawnCount);
	}

	return(0);
}