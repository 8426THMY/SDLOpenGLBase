#ifndef particleEmitter_h
#define particleEmitter_h


#include <stddef.h>

#include "particle.h"


typedef struct particleEmitter {
	// Time elapsed since the last "spawn wave".
	float elapsedTime;
	// Number of particles to emit per millisecond.
	float period;
} particleEmitter;

typedef struct particleEmitterDef {
	// This should be large enough
	// to store any type of emitter.
	union {
		//
	} data;

	// This function is executed when the emitter is updated and
	// returns how many new particles the emitter should spawn.
	size_t (*func)(particleEmitter *const restrict emitter);
} particleEmitterDef;


void particleEmitterInit(particleEmitter *const restrict emitter);

size_t particleEmitterUpdate(particleEmitter *const restrict emitter, const particleEmitterDef *const restrict emitterDef, const float dt);

size_t particleEmitterContinuous(particleEmitter *const restrict emitter);


#endif