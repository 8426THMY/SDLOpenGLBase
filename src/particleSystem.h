#ifndef particleSystem_h
#define particleSystem_h


#include <stddef.h>

#include "vec3.h"
#include "quat.h"

#include "particle.h"
#include "particleInitializer.h"
#include "particleEmitter.h"
#include "particleOperator.h"
#include "particleConstraint.h"


// A particle system cannot spawn any more than this number
// of particles, excluding those spawned by its children.
#define PARTICLESYSTEM_MAX_PARTICLES 1024


/*
** Children don't have to be rendered at exactly the same
** time as the rest of the particles/children in a system.
*/
typedef struct particleSystemDef particleSystemDef;
#warning "This isn't used at the moment. We also need a version for instantiated children."
typedef struct particleSystemChildDef {
	particleSystemDef *partSysDef;
	// How long to delay the rendering of
	// this system from the parent system.
	float delay;
} particleSystemChildDef;


/*
** Particle systems control the rendering and emission of a single particle
** particle effect. Although the particles may only use one texture, we can
** have any number of child systems that need not use the same texture.
*/
#warning "It'd also be nice if we had a way of deciding how many times a system should loop after all the particles have died."
typedef struct particleSystemDef {
	char *name;

	// Base particle properties.
	particleDef properties;
	// Number of particles to begin with.
	size_t initialParticles;
	// The maximum number of particles that the system can spawn, excluding those spawned
	// by its children. This can be no greater than "PARTICLESYSTEM_MAX_PARTICLES".
	size_t maxParticles;

	// These properties control the behaviour of the particles.
	particleInitializer *initializers;
	particleInitializer *lastInitializer;
	particleEmitterDef *emitters;
	size_t numEmitters;
	particleOperator *operators;
	particleOperator *lastOperator;
	particleConstraint *constraints;
	particleConstraint *lastConstraint;

	// How long the system should live for.
	// If set to INFINITY, it will never die.
	float lifetime;

	particleSystemDef *children;
	size_t numChildren;
} particleSystemDef;

// This is simply an instance of a particle system definition.
#warning "It'd be good if we had a way of choosing whether or not to sort particles. Of course, we'd still need to move dead particles to the end of the array."
typedef struct particleSystem particleSystem;
typedef struct particleSystem {
	particleSystemDef *partSysDef;
	particleEmitter *emitters;

	vec3 pos;
	quat rot;
	// How much longer the system should live for.
	float lifetime;

	// This array is always exactly big enough to store
	// the number of particles given by "maxParticles".
	particle *particles;
	// Current number of spawned particles.
	size_t numParticles;

	particleSystem *children;
} particleSystem;


void particleSysDefInit(particleSystemDef *partSysDef);
void particleSysInit(particleSystem *partSys, particleSystemDef *partSysDef);

return_t particleSysAlive(particleSystem *partSys, const float time);
void particleSysUpdate(particleSystem *partSys, const float time);
void particleSysDraw(const particleSystem *partSys, const float time);

void particleSysDelete(particleSystem *partSys);
void particleSysDefDelete(particleSystemDef *partSysDef);


/**
The particle system's main loop should look something like this:

Update {
	for all emitters {
		Spawn particles;
	}

	for all particles {
		if particle is not dead {
			Operate;
			Constrain;
		}else{
			kill;
		}
	}

	Sort particles;
}

// Should we sort the particles during the render stage?
// Seems like a lot of trouble to go to for little to no gain.
Render {
	Write particles to shader until we reach one with lifetime <= 0.f or camDistance == -INFINITY.
	//
}
**/


/**
Temporary references:

https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
http://nehe.gamedev.net/article/particle_systems/15008/
https://developer.valvesoftware.com/wiki/Particle_System_Overview
**/


#endif