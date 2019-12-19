#ifndef particleSystem_h
#define particleSystem_h


#include <stddef.h>

#include "particleEmitter.h"
#include "particle.h"


// A particle system cannot spawn any more than this number
// of particles, excluding those spawned by its children.
#define PARTICLESYSTEM_MAX_PARTICLES 1024


/** Temporary typedefs, these should be function pointers. Maybe make a separate file for them. **/
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
	// Mesh?
	// TextureGroup?
	// Colour?
	// What else? Maybe check Source's particle system.


	// These properties control the behaviour of the particles.
	particleEmitterDef *emitters;
	size_t numEmitters;
	particleOperator *operators;
	size_t numOperators;
	particleConstraint *constraints;
	size_t numConstraints;


	particleSystemChildDef children;
	size_t numChildren;


	// initial particle count?
	// The maximum number of particles that the system can spawn, excluding those spawned
	// by its children. This can be no greater than "PARTICLESYSTEM_MAX_PARTICLES".
	//
	// Because children always contribute one to this count,
	// this must also be greater than or equal to "numChildren".
	size_t maxParticles;
} particleSystemDef;

// This is simply an instance of a particle system definition.
typedef struct particleSystem particleSystem;
typedef struct particleSystem {
	particleSystemDef *partSysDef;
	particleEmitter *emitters;
	particleSystem *children;

	// This array is always exactly big enough to store
	// the number of particles given by "maxParticles".
	particle *particles;
} particleSystem;


/**
Temporary references:

https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
http://nehe.gamedev.net/article/particle_systems/15008/
https://developer.valvesoftware.com/wiki/Particle_System_Overview
**/


#endif