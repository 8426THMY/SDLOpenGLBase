#include "particleManager.h"


#include "memoryManager.h"


void particleManagerInit(particleManager *const restrict manager, const size_t maxParticles){
	// Allocate enough memory for the maximum number
	// of particles we expect to be alive at once.
	manager->particles = memoryManagerGlobalAlloc(
		sizeof(*manager->particles) * maxParticles
	);
	if(manager->particles == NULL){
		/** MALLOC FAILED **/
	}
	manager->numParticles = 0;
}


// Return how many free particles are left.
size_t particleManagerRemaining(const particleManager *const restrict manager, const size_t maxParticles){
	return(maxParticles - manager->numParticles);
}

/*
** Add "numParticles" many particles to the front of the array
** and return a pointer to the first new particle! We assume that
** whoever is using this structure has checked if there's room!
*/
particle *particleManagerAllocFront(particleManager *const restrict manager, const size_t numParticles){
	memmove(
		&manager->particles[numParticles], manager->particles,
		manager->numParticles * sizeof(*manager->particles)
	);
	manager->numParticles += numParticles;
	return(manager->particles);
}

/*
** Add "numParticles" many particles to the back of the array
** and return a pointer to the first new particle! We assume that
** whoever is using this structure has checked if there's room!
*/
particle *particleManagerAllocBack(particleManager *const restrict manager, const size_t numParticles){
	particle *firstParticle = &manager->particles[manager->numParticles];
	manager->numParticles += numParticles;
	return(firstParticle);
}

void particleManagerFree(particleManager *const restrict manager, particle *const restrict part){
	particleDelete(part);
	--manager->numParticles;
}


void particleManagerDelete(particleManager *const restrict manager){
	if(manager->particles != NULL){
		memoryManagerGlobalFree(manager->particles);
	}
}