#include "particleEmitter.h"


void particleEmitterInit(particleEmitter *emitter){
	emitter->lifetime = 0.f;
	emitter->emissionTime = 0.032f;
}


// Return the number of particles the emitter is allowed to spawn.
size_t particleEmitterUpdate(particleEmitter *emitter, const particleEmitterDef *emitterDef, const float time){
	emitter->lifetime += time;

	return((*emitterDef->func)(emitter));
}


size_t particleEmitterContinuous(particleEmitter *emitter){
	// Determine the number of particles to emit.
	const size_t spawnCount = emitter->lifetime / emitter->emissionTime;
	// Reduce the emitter's lifetime so we don't
	// spawn these particles again next time.
	emitter->lifetime -= ((float)spawnCount) * emitter->emissionTime;

	return(spawnCount);
}