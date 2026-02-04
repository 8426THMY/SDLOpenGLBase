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
	// Linked list of nodes.
	particleSystemNode *nodes;
} particleSubsystem;


typedef struct particle particle;
void particleSubsysInstantiate(
	particleSubsystem *const restrict subsys,
	const particleSystemNodeContainer *const restrict container,
	particle *const restrict parent
);
void particleSubsysUpdateParentPointers(particleSubsystem *const restrict subsys);
void particleSubsysUpdateParentTransforms(
	particleSubsystem *const restrict subsys,
	const transform *const restrict parentState
);
void particleSubsysOrphan(particleSubsystem *const restrict subsys);
void particleSubsysRemove(
	particleSubsystem *const restrict subsys,
	particleSystemNode *const restrict node
);


#endif