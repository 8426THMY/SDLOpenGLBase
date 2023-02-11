#ifndef particleSystemNodeGroup_h
#define particleSystemNodeGroup_h


#include <stddef.h>

#include "transform.h"


// Used to manage a doubly-linked list of particle system nodes
// that share the same owner. Note that the nodes themselves
// should be allocated by and stored within containers.
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSystemNodeGroup {
	particleSystemNode *nodes;
	size_t numNodes;
	// This is a pointer to the parent's transform state array.
	// The first element is the parent's current state, and the
	// second is its previous state. This saves us from storing
	// this pointer for every single particle system node.
	#warning "It might be better to move this back into the particle system node."
	#warning "In general, we expect to have many more particles than particle system nodes."
	#warning "Hence, putting it here might result in higher memory usage."
	const transform *state;
} particleSystemNodeGroup;


void particleSysNodeGroupInit(
	particleSystemNodeGroup *const restrict group,
	const transform *const restrict state
);

void particleSysNodeGroupPrepend(
	particleSystemNodeGroup *const restrict group,
	particleSystemNode *const restrict node
);

void particleSysNodeGroupOrphan(particleSystemNodeGroup *const restrict group);
void particleSysNodeGroupRemove(
	particleSystemNodeGroup *const restrict group,
	particleSystemNode *const restrict node
);


#endif