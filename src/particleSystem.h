#ifndef particleSystem_h
#define particleSystem_h


#include <stddef.h>

#include "vec3.h"
#include "quat.h"

#include "particleEmitter.h"
#include "particle.h"


// A particle system cannot spawn any more than this number
// of particles, excluding those spawned by its children.
#define PARTICLESYSTEM_MAX_PARTICLES 1024


/** Temporary typedefs, these should be function pointers. Maybe make a separate file for them. **/
typedef void* particleInitializer;
typedef void* particleOperator;
typedef void* particleConstraint;

/*
** Children don't have to be rendered at exactly the same
** time as the rest of the particles/children in a system.
*/
typedef struct particleSystemDef particleSystemDef;
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

	particleSystemChildDef *children;
	size_t numChildren;
} particleSystemDef;

// This is simply an instance of a particle system definition.
typedef struct particleSystem particleSystem;
typedef struct particleSystem {
	particleSystemDef *partSysDef;
	particleEmitter *emitters;
	particleSystem *children;

	vec3 pos;
	quat rot;

	// This array is always exactly big enough to store
	// the number of particles given by "maxParticles".
	particle *particles;
	// Current number of spawned particles.
	size_t numParticles;
} particleSystem;


void particleSysDefInit(particleSystemDef *partSysDef);

void particleSysUpdate(particleSystem *partSys, const float time);
void particleSysDraw(const particleSystem *partSys, const float time);


/**
Temporary references:

https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
http://nehe.gamedev.net/article/particle_systems/15008/
https://developer.valvesoftware.com/wiki/Particle_System_Overview
**/


#endif