#ifndef particleSubsystem_h
#define particleSubsystem_h


#include <stddef.h>

#include "transform.h"


// Used to manage a doubly-linked list of particle system
// nodes that share the same owner and live on the same level
// of the particle system tree. The nodes should be allocated
// and stored by a particle system container.
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSubsystem {
	// Stores the current and previous configurations of the
	// particle subsystem. This is typically updated by the owner.
	// Depending on the inheritance settings, we can either:
	//     1. Ignore this (never inherit).
	//     2. Add this to newly-created particles (inherit on create).
	//     3. Add this to particles when rendering (always inherit).
	// The owner is responsible for updating this.
	#warning "We should probably move this out."
	transform state[2];
	// Linked list of nodes.
	particleSystemNode *nodes;
	size_t numNodes;
} particleSubsystem;


void particleSubsysInit(particleSubsystem *const restrict subsys);

void particleSubsysPrepend(
	particleSubsystem *const restrict subsys,
	particleSystemNode *const restrict node
);

void particleSubsysOrphan(particleSubsystem *const restrict subsys);
void particleSubsysRemove(
	particleSubsystem *const restrict subsys,
	particleSystemNode *const restrict node
);


#endif