#ifndef particleSystemNodeContainer_h
#define particleSystemNodeContainer_h


#include "camera.h"

#include "spriteRenderer.h"


/*
** This represents a high-level node of the particle
** system, and contains each of the node's instances.
*/
typedef struct particleSystemNodeDef particleSystemNodeDef;
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSystemNodeContainer particleSystemNodeContainer;
typedef struct particleSystemNodeContainer {
	particleSystemNodeDef *nodeDef;
	particleSystemNode *instances;

	// Array of child containers.
	particleSystemNodeContainer *children;
} particleSystemNodeContainer;


particleSystemNodeContainer *particleSysNodeContainerInit(
	particleSystemNodeContainer *const restrict container,
	const particleSystemNodeDef *const restrict nodeDef,
	particleSystemNodeContainer *const restrict children
);
particleSystemNode *particleSysNodeContainerInstantiate(
	particleSystemNodeContainer *const restrict container
);

void particleSysNodeContainerUpdate(
	particleSystemNodeContainer *const restrict container,
	const camera *const restrict cam, const float dt
);
void particleSysNodeContainerBatch(
	const particleSystemNodeContainer *const restrict container,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
);

void particleSysNodeContainerDelete(
	particleSystemNodeContainer *const restrict container
);


#endif