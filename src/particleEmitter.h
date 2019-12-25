#ifndef particleEmitter_h
#define particleEmitter_h


#include <stddef.h>

#include "particle.h"


typedef struct particleEmitter {
	float lifetime;
	float emissionTime;
} particleEmitter;

typedef struct particleEmitterDef {
	// This should be large enough
	// to store any type of emitter.
	union {
		//
	} data;

	// This function is executed when the emitter is updated and
	// returns how many new particles the emitter should spawn.
	size_t (*func)(particleEmitter *emitter);
} particleEmitterDef;


void particleEmitterInit(particleEmitter *emitter);

size_t particleEmitterUpdate(particleEmitter *emitter, const particleEmitterDef *emitterDef, const float time);

size_t particleEmitterContinuous(particleEmitter *emitter);


#endif