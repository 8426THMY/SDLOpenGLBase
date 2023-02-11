#ifndef particleManager_h
#define particleManager_h


#include <stddef.h>

#include "particle.h"


typedef union particleFreeListNode particleFreeListNode;
typedef union particleFreeListNode {
	particleFreeListNode *nextFree;
	particle part;
} particleFreeListNode;

// Stores a memory pool of particles, with free blocks handled
// using a free list. This structure also sorts our particles.
// This is kind of similar to the method suggested here:
//     http://gameprogrammingpatterns.com/object-pool.html
typedef struct particleManager {
	// Contiguous array of particles. The size of this array is
	// always equal to the definition's "maxParticles" value.
	// Too manage particles in this array, we use a free list.
	particleFreeListNode *particles;
	// Pointer to the next free element in the array.
	particleFreeListNode *nextFree;
	size_t numParticles;

	// The elements in the array above are not sorted in
	// any particular order. Rather than copying elements
	// around to sort it, particles store pointers to the
	// next and previous elements in the list. These point
	// to the first and last elements in the sorted list.
	particle *first;
	particle *last;
} particleManager;


void particleManagerInit(particleManager *const restrict manager, const size_t maxParticles);

size_t particleManagerRemaining(const particleManager *const restrict manager, const size_t maxParticles);
particle *particleManagerAlloc(particleManager *const restrict manager);
void particleManagerFree(particleManager *const restrict manager, particle *const restrict part);

void particleManagerDelete(particleManager *const restrict manager);


#endif


#warning "See the comment on the structure of arrays pattern in 'particle.h'."
#warning "While this would almost certainly reduce memory usage in large systems, it has some problems."
#warning "For instance, how do we efficiently sort particles like this?"
#warning "Perhaps we could have a structure that contains the data used by all particle system nodes."
#warning "This would be used for the free list, and would include 'prev' and 'next' indices for sorting."

#if 0
// Rather than storing an array of particle structures,
// we store a structure containing arrays of each possible
// field that a particle can use. Not only does this improve
// cache performance, it also means that we aren't storing
// fields that the particle system node isn't using.
typedef struct particleSystemNode particleSystemNode;
typedef struct particleFields {
	// Note: In order to correctly transform particles,
	//       we need to store their current and previous
	//       transformations. Thus, these arrays have
	//       twice as many elements as all of the others.
	vec3 *pos;
	quat *rot;
	vec3 *scale;
	vec3 *linearVelocity;
	vec3 *angularVelocity;
	vec3 *netForce;
	vec3 *netTorque;
	#warning "Depending on the renderer, we might want to store colours instead of texture data."
	#if 0
	// Animation progress.
	textureGroupAnim *animData;
	#endif
	// Although this will usually represent how much longer
	// the particle may live for, it can also represent other
	// things, such as how long it has been alive for.
	float *lifetime;
	// For each particle, we store the indices of the
	// particles before and after it in sorted order.
	size_t *prev;
	size_t *next;
	// Child particle subsystems.
	particleSystemNode **children;
	size_t *numChildren;
} particleFields;
#endif