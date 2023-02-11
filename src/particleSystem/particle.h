#ifndef particle_h
#define particle_h


#include <stddef.h>

#include "vec3.h"
#include "transform.h"
#include "textureGroup.h"

#include "particleSystemNodeGroup.h"

#include "utilTypes.h"


#warning "It might be more efficient to use a 'structure of arrays' pattern to store particles."
#warning "That is, rather than nodes storing an array of particles, they have a single 'particleData' structure."
#warning "This structure would store an array of transform states, velocities, lifetimes, etcetera."
#warning "However, depending on how the particles behave, we don't need to allocate all of these arrays."
#warning "This technique is outlined here:"
//https://www.cppstories.com/2014/04/flexible-particle-system-container-2/


typedef struct particle particle;
typedef struct particle {
	// Current and previous global transforms.
	#warning "We shouldn't allow particles to be sheared, as it's way too slow."
	#warning "If we're desperate, this can be emulated using animated textures or something."
	#warning "We should be able to solve this when we convert to a structure of arrays."
	#warning "In fact, particles shouldn't even inherit scale at all, meaning we don't need shearing!"
	#warning "As far as scaling is concerned, particles should probably only use a single float."
	transform state[2];

	// These roperties control the body's motion.
	vec3 linearVelocity;
	vec3 angularVelocity;
	vec3 netForce;
	vec3 netTorque;

	#warning "Depending on the renderer, we might want to store colours instead of texture data."
	#if 0
	// Animation progress.
	textureGroupAnim animData;
	#endif

	// Although this will usually represent how much longer
	// the particle may live for, it can also represent other
	// things, such as how long it has been alive for.
	float lifetime;

	// Pointers to the previous and next
	// particles in the sorted list.
	particle *prev;
	particle *next;

	// Manages child particle subsystems.
	particleSystemNodeGroup group;
} particle;


void particlePreInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
);
void particlePostInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
);

void particlePreUpdate(
	particle *const restrict part,
	const transform *const restrict parentState,
	const float dt
);
void particlePostUpdate(particle *const restrict part, const float dt);
return_t particleDead(const particle *const restrict part);

void particleDelete(particle *const restrict part);


#endif