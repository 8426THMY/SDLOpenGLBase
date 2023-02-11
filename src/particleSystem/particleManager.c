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
	// Initialize the free list.
	{
		particleFreeListNode *curNode = manager->particles;
		const particleFreeListNode *const lastNode = &curNode[maxParticles - 1];
		// Each node should point to the next one by default.
		while(curNode < lastNode){
			curNode->nextFree = ++curNode;
		}
		curNode->nextFree = NULL;
	}
	manager->nextFree = manager->particles;
	manager->numParticles = 0;

	manager->first = NULL;
	manager->last = NULL;
}


// Return how many free particles are left.
size_t particleManagerRemaining(const particleManager *const restrict manager, const size_t maxParticles){
	return(maxParticles - manager->numParticles);
}

/*
** We assume that whoever is using this structure has
** checked if there's room to allocate a new particle!
*/
particle *particleManagerAlloc(particleManager *const restrict manager){
	// Take the next free node in the list.
	particleFreeListNode *const node = manager->nextFree;
	manager->nextFree = node->nextFree;
	++manager->numParticles;

	// Set the particle's sort pointers.
	node->part.prev = manager->first;
	node->part.next = NULL;

	// If the sorted list is empty, add this particle to the beginning.
	if(manager->first == NULL){
		manager->first = &node->part;
	}
	// The particle is always added to the end.
	manager->last = &node->part;

	return(&node->part);
}

/*
** We assume that whoever is using this structure has
** checked if there's room to allocate a new particle!
*/
void particleManagerFree(particleManager *const restrict manager, particle *const restrict part){
	// Fix up the particle's pointers.
	if(part->prev == NULL){
		manager->first = part->next;
	}else{
		part->prev->next = part->next;
	}
	if(part->next == NULL){
		manager->last = part->prev;
	}else{
		part->next->prev = part->prev;
	}
	// Delete the particle.
	particleDelete(part);

	// Add the particle's node to the beginning of the free list.
	((particleFreeListNode *)part)->nextFree = manager->nextFree;
	manager->nextFree = (particleFreeListNode *)part;
	--manager->numParticles;
}


void particleManagerDelete(particleManager *const restrict manager){
	if(manager->particles != NULL){
		memoryManagerGlobalFree(manager->particles);
	}
}