#ifndef particleManager_h
#define particleManager_h


#include <stddef.h>

#include "particle.h"


// A particle manager is really just a glorified array of particles.
typedef struct particleManager {
	// Contiguous array of particles. The size of this array is
	// always equal to the definition's "maxParticles" value.
	particle *particles;
	size_t numParticles;
} particleManager;


void particleManagerInit(particleManager *const restrict manager, const size_t maxParticles);

size_t particleManagerRemaining(const particleManager *const restrict manager, const size_t maxParticles);
particle *particleManagerAllocFront(particleManager *const restrict manager, const size_t numParticles);
particle *particleManagerAllocBack(particleManager *const restrict manager, const size_t numParticles);
void particleManagerFree(particleManager *const restrict manager, particle *const restrict part);

void particleManagerDelete(particleManager *const restrict manager);


#endif


#warning "See the comment on the structure of arrays pattern in 'particle.h'."
#warning "While this would almost certainly reduce memory usage in large systems, it has some problems."
#warning "For instance, how do we efficiently sort particles like this?"
#warning "The easiest way would probably be to store all of the arrays in sorted order."
#warning "This would be slower, but it could have potential benefits for the cache."

#if 0
#define PARTICLE_STORE_ROTATION         0x01
#define PARTICLE_STORE_SCALE            0x02
#define PARTICLE_STORE_LINEAR_VELOCITY  0x04
#define PARTICLE_STORE_ANGULAR_VELOCITY 0x08
#define PARTICLE_STORE_FORCE            0x10
#define PARTICLE_STORE_TORQUE           0x20
#define PARTICLE_STORE_COLOUR           0x40
#define PARTICLE_STORE_ANIMATION        0x80

// Rather than storing an array of particle structures,
// we store a structure containing arrays of each possible
// field that a particle can use. Not only does this improve
// cache performance, it also means that we aren't storing
// fields that the particle system node isn't using.
typedef struct particleSystemNode particleSystemNode;
typedef struct particleFields {
	// These properties describe the particle's previous
	// and current global configurations. That is, these
	// arrays are twice as large as the others.
	vec3 *position;
	quat *rotation;
	float *scale;
	// Physics properties.
	vec3 *linearVelocity;
	vec3 *angularVelocity;
	vec3 *netForce;
	vec3 *netTorque;
	// Renderer properties.
	vec3 *colour;
	#if 0
	textureGroupAnim *animData;
	#endif
	// Although this will usually represent how much longer
	// the particle may live for, it can also represent other
	// things, such as how long it has been alive for.
	float *lifetime;
	// Particles can store nodes that are lower in the tree.
	particleSubsystem *subsys;

	size_t numParticles;
} particleFields;
#endif